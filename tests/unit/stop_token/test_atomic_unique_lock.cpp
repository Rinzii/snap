#include <gtest/gtest.h>

#include "snap/stop_token/atomic_unique_lock.hpp"

#include <atomic>

TEST(StopToken, AtomicUniqueLockSetsAndClearsBit)
{
        std::atomic<unsigned int> state{ 0 };
        {
                snap::atomic_unique_lock<unsigned int, 1u> lock(state);
                EXPECT_TRUE(lock.owns_lock());
                EXPECT_EQ(state.load(std::memory_order_relaxed) & 1u, 1u);
        }
        EXPECT_EQ(state.load(std::memory_order_relaxed) & 1u, 0u);
}
