#ifndef SNP_INCLUDE_SNAP_STOP_TOKEN_ATOMIC_UNIQUE_LOCK_HPP
#define SNP_INCLUDE_SNAP_STOP_TOKEN_ATOMIC_UNIQUE_LOCK_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/popcount.hpp"
#include "snap/internal/helpers/atomic_helpers.hpp"

#include <atomic>
#include <utility>

SNAP_BEGIN_NAMESPACE

// RAII lock built on a single atomic state word.
// The lock is represented by one bit (LockedBit) inside State.
template <class State, State LockedBit> class atomic_unique_lock
{
	// LockedBit must be a one-hot mask, so we can set/clear it without affecting other bits.
	static_assert(popcount(static_cast<unsigned long long>(LockedBit)) == 1, "LockedBit must be an integer where only one bit is set");

public:
	// Locks unconditionally on construction.
	explicit atomic_unique_lock(std::atomic<State>& state) noexcept : state_(state), is_locked_(false) { lock(); }

	// Attempts to lock, but allows the caller to give up based on the observed state.
	// If our give_up_locking(current_state) returns true, the constructor finishes without owning the lock.
	template <class Pred> atomic_unique_lock(std::atomic<State>& state, Pred&& give_up_locking) noexcept : state_(state), is_locked_(false)
	{
		is_locked_ = lock_impl(std::forward<Pred>(give_up_locking), set_locked_bit, std::memory_order_acquire);
	}

	// Attempts to lock with:
	// - a give-up predicate
	// - a custom function that computes the post-lock state (lets callers set other bits atomically with the lock bit)
	// - a selectable ordering for the successful lock acquisition
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

	// Unlocks on destruction if we still own the lock.
	~atomic_unique_lock()
	{
		if (is_locked_) { unlock(); }
	}

	// Reports whether this instance currently owns the lock.
	bool owns_lock() const noexcept { return is_locked_; }

	// Unconditional lock acquisition (never gives up).
	// Acquire ordering ensures reads after lock() observe prior writes released by the previous owner.
	void lock() noexcept
	{
		// Predicate used by the unconditional path: never give up.
		auto never_give_up = [](State) { return false; };

		// lock_impl returns whether the lock was acquired; for this path it should always succeed.
		[[maybe_unused]] const bool locked = lock_impl(never_give_up, set_locked_bit, std::memory_order_acquire);

		// Record ownership for the destructor and owns_lock().
		is_locked_ = true;
	}

	// Releases the lock by clearing LockedBit and waking any waiters.
	// Release ordering publishes writes in the critical section to the next owner.
	void unlock() noexcept
	{
		// Clear the lock bit without disturbing other state bits.
		state_.fetch_and(static_cast<State>(~LockedBit), std::memory_order_release);

		// Wake threads blocked in lock_impl() waiting on this state word.
		internal::atomic_notify_all(state_);

		is_locked_ = false;
	}

private:
	// Core lock algorithm:
	// - load the current state
	// - if locked, wait until state changes
	// - otherwise attempt CAS to set the lock bit (and optionally other bits)
	// - on CAS failure, retry with the updated observed state
	template <class Pred, class UnaryFunction>
	bool lock_impl(Pred&& give_up_locking, UnaryFunction&& state_after_lock, std::memory_order locked_ordering) noexcept
	{
		// Relaxed is enough for polling; the CAS enforces ordering on success and by the unlocked release.
		State current = state_.load(std::memory_order_relaxed);

		for (;;)
		{
			for (;;)
			{
				// Caller-defined early exit (only used by the optional constructors).
				if (give_up_locking(current)) { return false; }

				// If another thread holds the lock, wait until the state changes.
				if ((current & LockedBit) != 0)
				{
					// Uses std::atomic::wait on C++20, otherwise uses the internal parking-lot fallback.
					internal::atomic_wait(state_, current, std::memory_order_relaxed);

					// After waking (or spuriously), reload and re-check the lock bit.
					current = state_.load(std::memory_order_relaxed);
					continue;
				}

				// Observed unlocked state; proceed to CAS.
				break;
			}

			// Attempt to transition from current -> state_after_lock(current).
			// On failure, current is updated with the new observed value and the loop retries.
			if (state_.compare_exchange_weak(current, state_after_lock(current), locked_ordering, std::memory_order_relaxed)) { return true; }
		}
	}

	// Default post-lock transform: set the lock bit, preserving any other bits.
	static constexpr auto set_locked_bit = [](State s) { return static_cast<State>(s | LockedBit); };

	std::atomic<State>& state_;
	bool is_locked_;
};

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_STOP_TOKEN_ATOMIC_UNIQUE_LOCK_HPP
