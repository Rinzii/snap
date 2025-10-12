#pragma once

// C++17 stop state using snap::intrusive_list_view (converted earlier) and
// snap::atomic_unique_lock. No reserved identifiers.

#include "snap/atomic_unique_lock.hpp"	// atomic_unique_lock<State, LockedBit>
#include "snap/intrusive_list_view.hpp" // intrusive_node_base<T>, intrusive_list_view<T>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <mutex>
#include <thread>
#include <utility>

namespace snap
{

	template <class T> struct intrusive_shared_ptr_traits;

	// Node base for registered callbacks.
	struct stop_callback_base : intrusive_node_base<stop_callback_base>
	{
		using callback_fn = void (*)(stop_callback_base*) noexcept;

		explicit stop_callback_base(callback_fn fn) noexcept : fn_(fn) {}

		void invoke() noexcept { fn_(this); }

		callback_fn fn_				= nullptr;
		std::atomic<bool> completed = false;
		bool* destroyed				= nullptr;

		std::mutex completed_mtx;
		std::condition_variable completed_cv;
	};

	// Shared stop state (intrusive-refcounted via intrusive_shared_ptr_traits).
	class stop_state
	{
		// [ ... stop_source counter ... | lock bit | stop_requested ]
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
			state_t cur = state_.load(std::memory_order_relaxed);
			(void)cur;
			assert((cur >> stop_source_counter_shift) != (std::numeric_limits<state_t>::max() >> stop_source_counter_shift) && "stop_source counter overflow");
			state_.fetch_add(state_t(1u << stop_source_counter_shift), std::memory_order_relaxed);
		}

		void decrement_stop_source_counter() noexcept
		{
			state_t cur = state_.load(std::memory_order_relaxed);
			(void)cur;
			assert((cur >> stop_source_counter_shift) > 0 && "stop_source counter underflow");
			state_.fetch_sub(state_t(1u << stop_source_counter_shift), std::memory_order_relaxed);
		}

		bool stop_requested() const noexcept { return (state_.load(std::memory_order_acquire) & stop_requested_bit) != 0; }

		bool stop_possible_for_token() const noexcept
		{
			state_t s = state_.load(std::memory_order_acquire);
			return (s & stop_requested_bit) != 0 || ((s >> stop_source_counter_shift) != 0);
		}

		// Returns true if transitioned to requested and invoked callbacks.
		bool request_stop() noexcept
		{
			auto lock = try_lock_for_request_stop();
			if (!lock.owns_lock()) return false;

			requesting_thread_ = std::this_thread::get_id();

			while (!callbacks_.empty())
			{
				auto* node = callbacks_.pop_front(); // marks node as detached (prev == nullptr)
				lock.unlock();

				bool destroyed_flag = false;
				node->destroyed		= &destroyed_flag;

				node->invoke();

				if (!destroyed_flag)
				{
					node->destroyed = nullptr;
					{
						std::lock_guard<std::mutex> g(node->completed_mtx);
						node->completed.store(true, std::memory_order_release);
					}
					node->completed_cv.notify_all();
				}

				lock.lock();
			}

			return true;
		}

		// If already requested: runs cb immediately and returns false.
		// If no stop_source present: returns false (cannot ever run).
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
			if (!lock.owns_lock()) return false;

			callbacks_.push_front(cb);
			return true;
		}

		// Called by stop_callback destructor.
		void remove_callback(stop_callback_base* cb) noexcept
		{
			list_lock lock(state_);

			// If prev == nullptr (detached by pop_front) and not the sentinel, it may be executing now.
			const bool potentially_executing_now = cb->prev == nullptr && !callbacks_.is_head(cb);

			if (potentially_executing_now)
			{
				auto requester = requesting_thread_;
				lock.unlock();

				if (std::this_thread::get_id() != requester)
				{
					std::unique_lock<std::mutex> g(cb->completed_mtx);
					cb->completed_cv.wait(g, [&] { return cb->completed.load(std::memory_order_acquire); });
				}
				else
				{
					if (cb->destroyed) *cb->destroyed = true;
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
		std::atomic<state_t> ref_count_{ 0 }; // for intrusive_shared_ptr
		callback_list callbacks_{};
		std::thread::id requesting_thread_{};

		template <class T> friend struct intrusive_shared_ptr_traits;
	};

	// intrusive_shared_ptr trait specialization for stop_state.
	template <> struct intrusive_shared_ptr_traits<stop_state>
	{
		static std::atomic<std::uint32_t>& get_atomic_ref_count(stop_state& s) noexcept { return s.ref_count_; }
	};

} // namespace snap
