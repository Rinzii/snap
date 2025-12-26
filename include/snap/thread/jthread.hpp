#ifndef SNP_INCLUDE_SNAP_THREAD_JTHREAD_HPP
#define SNP_INCLUDE_SNAP_THREAD_JTHREAD_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/stop_token/stop_source.hpp"
#include "snap/type_traits/remove_cvref.hpp"

#include <thread>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE

class jthread
{
public:
	using id				 = std::thread::id;
	using native_handle_type = std::thread::native_handle_type;

	jthread() noexcept : stop_source_(nostopstate) {}

	template <class Fun, class... Args, class = std::enable_if_t<!std::is_same_v<remove_cvref_t<Fun>, jthread>>> explicit jthread(Fun&& fun, Args&&... args)
		: stop_source_(), thread_(init_thread(stop_source_, std::forward<Fun>(fun), std::forward<Args>(args)...))
	{
		static_assert(std::is_constructible_v<std::decay_t<Fun>, Fun>, "Fun must be constructible from the provided callable");
		static_assert((std::is_constructible_v<std::decay_t<Args>, Args> && ...), "Each Arg must be constructible from the provided argument");
		static_assert(std::is_invocable_v<std::decay_t<Fun>, std::decay_t<Args>...> ||
						  std::is_invocable_v<std::decay_t<Fun>, stop_token, std::decay_t<Args>...>,
					  "Callable must be invocable with (Args...) or (stop_token, Args...)");
	}

	~jthread()
	{
		if (joinable())
		{
			request_stop();
			join();
		}
	}

	jthread(const jthread&)			   = delete;
	jthread& operator=(const jthread&) = delete;

	jthread(jthread&&) noexcept = default;

	jthread& operator=(jthread&& other) noexcept
	{
		if (this != &other)
		{
			if (joinable())
			{
				request_stop();
				join();
			}
			stop_source_ = std::move(other.stop_source_);
			thread_		 = std::move(other.thread_);
		}
		return *this;
	}

	void swap(jthread& other) noexcept
	{
		using std::swap;
		swap(stop_source_, other.stop_source_);
		swap(thread_, other.thread_);
	}

	[[nodiscard]] bool joinable() const noexcept { return get_id() != id(); }

	void join() { thread_.join(); }
	void detach() { thread_.detach(); }

	[[nodiscard]] id get_id() const noexcept { return thread_.get_id(); }

	[[nodiscard]] native_handle_type native_handle() { return thread_.native_handle(); }

	[[nodiscard]] stop_source get_stop_source() noexcept { return stop_source_; }
	[[nodiscard]] stop_token get_stop_token() const noexcept { return stop_source_.get_token(); }
	bool request_stop() noexcept { return stop_source_.request_stop(); }

	friend void swap(jthread& a, jthread& b) noexcept { a.swap(b); }

	[[nodiscard]] static unsigned int hardware_concurrency() noexcept { return std::thread::hardware_concurrency(); }

private:
	template <class Fun, class... Args> static std::thread init_thread(const stop_source& ss, Fun&& fun, Args&&... args)
	{
		using FunD = std::decay_t<Fun>;

		// ReSharper disable once CppTooWideScope
		constexpr bool takes_token = std::is_invocable_v<FunD, stop_token, std::decay_t<Args>...>;

		if constexpr (takes_token) { return std::thread(std::forward<Fun>(fun), ss.get_token(), std::forward<Args>(args)...); }
		else { return std::thread(std::forward<Fun>(fun), std::forward<Args>(args)...); }
	}

	stop_source stop_source_{};
	std::thread thread_{};
};

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_THREAD_JTHREAD_HPP
