#include "snap/thread/jthread.hpp"

#include <gtest/gtest.h>

#include <atomic>

TEST(Thread, JthreadInvokesCallableWithStopToken)
{
	std::atomic<bool> stop_possible{ false };
	{
		snap::jthread worker([&](snap::stop_token token) { stop_possible.store(token.stop_possible(), std::memory_order_relaxed); });
		EXPECT_TRUE(worker.joinable());
		EXPECT_TRUE(worker.request_stop());
		worker.join();
	}
	EXPECT_TRUE(stop_possible.load(std::memory_order_relaxed));
}
