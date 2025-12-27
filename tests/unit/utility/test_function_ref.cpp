#include <gtest/gtest.h>

#include <snap/internal/abi_namespace.hpp>
#include <snap/utility/function_ref.hpp>

#include <functional>

namespace
{
	int add_pair(int a, int b)
	{
		return a + b;
	}

	struct Tracker
	{
		int value = 0;

		int multiply(int input) { return value * input; }
	};
} // namespace

TEST(UtilityFunctionRef, InvokesLvalueCallable)
{
	int sum			= 0;
	auto accumulate = [&sum](int v) { sum += v; };

	SNAP_NAMESPACE::function_ref<void(int)> ref(accumulate);
	ASSERT_TRUE(ref);
	ref(4);
	ref(3);
	EXPECT_EQ(sum, 7);
}

TEST(UtilityFunctionRef, WrapsFunctionPointer)
{
	SNAP_NAMESPACE::function_ref<int(int, int)> ref(add_pair);
	EXPECT_EQ(ref(5, 7), 12);
}

TEST(UtilityFunctionRef, WorksWithMemberAccessThroughLambda)
{
	Tracker tracker{ 3 };
	auto lambda = [&tracker](int x) { return tracker.multiply(x); };

	SNAP_NAMESPACE::function_ref<int(int)> ref(lambda);
	EXPECT_EQ(ref(5), 15);
}

TEST(UtilityFunctionRef, SupportsNoexceptSignatures)
{
	int calls = 0;
	auto ping = [&calls]() noexcept { ++calls; };

	SNAP_NAMESPACE::function_ref<void() noexcept> ref(ping);
	static_assert(noexcept(ref()));
	ref();
	EXPECT_EQ(calls, 1);
}
