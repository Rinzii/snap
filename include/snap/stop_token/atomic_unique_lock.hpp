#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <atomic>
#include <thread>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
// RAII lock over an atomic<State>. Locks by setting a single-bit mask (LockedBit).
// Pred/give-up and state_after_lock let callers customize behavior.
template <class State, State LockedBit> class atomic_unique_lock
{
	static_assert(LockedBit != 0 && (LockedBit & (LockedBit - 1)) == 0, "LockedBit must have exactly one bit set");

public:
	explicit atomic_unique_lock(std::atomic<State>& state) noexcept : state_(state), is_locked_(false) { lock(); }

	template <class Pred> atomic_unique_lock(std::atomic<State>& state, Pred&& give_up_locking) noexcept : state_(state), is_locked_(false)
	{
		is_locked_ = lock_impl(std::forward<Pred>(give_up_locking), set_locked_bit, std::memory_order_acquire);
	}

	template <class Pred, class UnaryFunction>
	atomic_unique_lock(std::atomic<State>& state, Pred&& give_up_locking, UnaryFunction&& state_after_lock, std::memory_order locked_ordering) noexcept
		: state_(state), is_locked_(false)
	{
		is_locked_ = lock_impl(std::forward<Pred>(give_up_locking), std::forward<UnaryFunction>(state_after_lock), locked_ordering);
	}

	atomic_unique_lock(const atomic_unique_lock&)			 = delete;
	atomic_unique_lock& operator=(const atomic_unique_lock&) = delete;
	atomic_unique_lock(atomic_unique_lock&&)				 = delete;
	atomic_unique_lock& operator=(atomic_unique_lock&&)		 = delete;

	~atomic_unique_lock()
	{
		if (is_locked_) { unlock(); }
	}

	bool owns_lock() const noexcept { return is_locked_; }

	void lock() noexcept
	{
		auto never_give_up = [](State) { return false; };
		// Acquire: subsequent reads see up-to-date values after locking.
		(void)lock_impl(never_give_up, set_locked_bit, std::memory_order_acquire);
		is_locked_ = true;
	}

	void unlock() noexcept
	{
		// Release: prior writes become visible before clearing the bit.
		state_.fetch_and(static_cast<State>(~LockedBit), std::memory_order_release);
		// C++17 has no atomic::notify_*; waiters spin and observe the cleared bit.
		is_locked_ = false;
	}

private:
	template <class Pred, class UnaryFunction>
	bool lock_impl(Pred&& give_up_locking, UnaryFunction&& state_after_lock, std::memory_order locked_ordering) noexcept
	{
		State current = state_.load(std::memory_order_relaxed);
		for (;;)
		{
			while (true)
			{
				if (give_up_locking(current))
				{
					return false; // caller asked to give up
				}
				if ((current & LockedBit) != 0)
				{
					backoff();
					current = state_.load(std::memory_order_relaxed);
				}
				else
				{
					break; // try to take the lock
				}
			}

			// Attempt to set the lock bit (and possibly other bits) atomically.
			if (state_.compare_exchange_weak(current, state_after_lock(current), locked_ordering, std::memory_order_relaxed))
			{
				return true; // locked
			}
			// On failure, current has been updated; loop and retry.
		}
	}

	static constexpr auto set_locked_bit = [](State s) { return static_cast<State>(s | LockedBit); };

	static void backoff() noexcept
	{
		// Lightweight backoff suitable for C++17 (portable).
		// Yielding reduces contention without busy-burning a full core.
		std::this_thread::yield();
	}

	std::atomic<State>& state_;
	bool is_locked_;
};

SNAP_END_NAMESPACE
