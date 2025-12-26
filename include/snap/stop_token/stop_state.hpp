#ifndef SNP_INCLUDE_SNAP_STOP_TOKEN_STOP_STATE_HPP
#define SNP_INCLUDE_SNAP_STOP_TOKEN_STOP_STATE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/helpers/atomic_helpers.hpp"
#include "snap/stop_token/atomic_unique_lock.hpp"
#include "snap/stop_token/intrusive_list_view.hpp"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <limits>
#include <thread>

SNAP_BEGIN_NAMESPACE

template <class T> struct intrusive_shared_ptr_traits;

struct stop_callback_base : intrusive_node_base<stop_callback_base>
{
	using callback_fn = void (*)(stop_callback_base*) noexcept;

	explicit stop_callback_base(callback_fn fn) noexcept : fn_(fn) {}

	void invoke() noexcept { fn_(this); }

	callback_fn fn_ = nullptr;

	std::atomic<bool> completed{ false };
	bool* destroyed = nullptr;
};

class stop_state
{
	static constexpr std::uint32_t stop_requested_bit		 = 1u;
	static constexpr std::uint32_t callback_list_locked_bit	 = 1u << 1;
	static constexpr std::uint32_t stop_source_counter_shift = 2u;

	using state_t		= std::uint32_t;
	using list_lock		= atomic_unique_lock<state_t, callback_list_locked_bit>;
	using callback_list = intrusive_list_view<stop_callback_base>;

public:
	stop_state() noexcept = default;

	void increment_stop_source_counter() noexcept
	{
		const state_t cur = state_.load(std::memory_order_relaxed);
		(void)cur;
		assert((cur >> stop_source_counter_shift) != (std::numeric_limits<state_t>::max() >> stop_source_counter_shift) && "stop_source counter overflow");
		state_.fetch_add(state_t(1u << stop_source_counter_shift), std::memory_order_relaxed);
	}

	void decrement_stop_source_counter() noexcept
	{
		const state_t cur = state_.load(std::memory_order_relaxed);
		(void)cur;
		assert((cur >> stop_source_counter_shift) > 0 && "stop_source counter underflow");
		state_.fetch_sub(state_t(1u << stop_source_counter_shift), std::memory_order_relaxed);
	}

	bool stop_requested() const noexcept { return (state_.load(std::memory_order_acquire) & stop_requested_bit) != 0; }

	bool stop_possible_for_token() const noexcept
	{
		const state_t s = state_.load(std::memory_order_acquire);
		return (s & stop_requested_bit) != 0 || ((s >> stop_source_counter_shift) != 0);
	}

	bool request_stop() noexcept
	{
		auto lock = try_lock_for_request_stop();
		if (!lock.owns_lock()) { return false; }

		requesting_thread_ = std::this_thread::get_id();

		while (!callbacks_.empty())
		{
			stop_callback_base* cb = callbacks_.pop_front();

			lock.unlock();

			bool destroyed_flag = false;
			cb->destroyed		= &destroyed_flag;

			cb->invoke();

			if (!destroyed_flag)
			{
				cb->destroyed = nullptr;

				cb->completed.store(true, std::memory_order_release);
				internal::atomic_notify_all(cb->completed);
			}

			lock.lock();
		}

		return true;
	}

	bool add_callback(stop_callback_base* cb) noexcept
	{
		const auto give_up = [cb](state_t s)
		{
			if ((s & stop_requested_bit) != 0)
			{
				cb->invoke();
				return true;
			}
			return (s >> stop_source_counter_shift) == 0;
		};

		list_lock lock(state_, give_up);
		if (!lock.owns_lock()) { return false; }

		callbacks_.push_front(cb);
		return true;
	}

	void remove_callback(stop_callback_base* cb) noexcept
	{
		list_lock lock(state_);

		const bool potentially_executing_now = cb->prev == nullptr && !callbacks_.is_head(cb);

		if (potentially_executing_now)
		{
			const auto requester = requesting_thread_;
			lock.unlock();

			if (std::this_thread::get_id() != requester) { internal::atomic_wait(cb->completed, false, std::memory_order_acquire); }
			else
			{
				if (cb->destroyed) { *cb->destroyed = true; }
			}
		}
		else { callbacks_.remove(cb); }
	}

private:
	list_lock try_lock_for_request_stop() noexcept
	{
		const auto lock_fail  = [](state_t s) { return (s & stop_requested_bit) != 0; };
		const auto after_lock = [](state_t s) { return static_cast<state_t>(s | callback_list_locked_bit | stop_requested_bit); };
		return list_lock(state_, lock_fail, after_lock, std::memory_order_acq_rel);
	}

	std::atomic<state_t> state_{ 0 };
	std::atomic<state_t> ref_count_{ 0 };
	callback_list callbacks_{};
	std::thread::id requesting_thread_{};

	template <class T> friend struct intrusive_shared_ptr_traits;
};

template <> struct intrusive_shared_ptr_traits<stop_state>
{
	static std::atomic<std::uint32_t>& get_atomic_ref_count(stop_state& s) noexcept { return s.ref_count_; }
};

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_STOP_TOKEN_STOP_STATE_HPP
