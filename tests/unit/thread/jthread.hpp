#pragma once

#include "snap/internal/abi_namespace.hpp"

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

		template <class Fun, class... Args, class = typename std::enable_if<!std::is_same<typename std::decay<Fun>::type, jthread>::value>::type>
		explicit jthread(Fun&& fun, Args&&... args) : stop_source_(), thread_(init_thread(stop_source_, std::forward<Fun>(fun), std::forward<Args>(args)...))
		{
			static_assert(std::is_constructible<typename std::decay<Fun>::type, Fun>::value, "Fun must be constructible from the provided callable");
			static_assert((std::is_constructible<typename std::decay<Args>::type, Args>::value && ...),
						  "Each Arg must be constructible from the provided argument");
			static_assert(std::is_invocable<typename std::decay<Fun>::type, typename std::decay<Args>::type...>::value ||
							  std::is_invocable<typename std::decay<Fun>::type, snap::stop_token, typename std::decay<Args>::type...>::value,
						  "Callable must be invocable with (Args...) or (snap::stop_token, Args...)");
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

		bool joinable() const noexcept { return thread_.joinable(); }

		void join() { thread_.join(); }
		void detach() { thread_.detach(); }

		id get_id() const noexcept { return thread_.get_id(); }

		native_handle_type native_handle() { return thread_.native_handle(); }

		// stop handling
		snap::stop_source get_stop_source() noexcept { return stop_source_; }
		snap::stop_token get_stop_token() const noexcept { return stop_source_.get_token(); }
		bool request_stop() noexcept { return stop_source_.request_stop(); }

		friend void swap(jthread& a, jthread& b) noexcept { a.swap(b); }

		static unsigned int hardware_concurrency() noexcept { return std::thread::hardware_concurrency(); }

	private:
		template <class Fun, class... Args> static std::thread init_thread(const snap::stop_source& ss, Fun&& fun, Args&&... args)
		{
			using FunD				   = typename std::decay<Fun>::type;
			constexpr bool takes_token = std::is_invocable<FunD, snap::stop_token, typename std::decay<Args>::type...>::value;

			if constexpr (takes_token) { return std::thread(std::forward<Fun>(fun), ss.get_token(), std::forward<Args>(args)...); }
			else { return std::thread(std::forward<Fun>(fun), std::forward<Args>(args)...); }
		}

		snap::stop_source stop_source_{};
		std::thread thread_{};
	};

SNAP_END_NAMESPACE
