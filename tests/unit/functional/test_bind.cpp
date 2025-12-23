#include <snap/internal/abi_namespace.hpp>

#include <snap/functional/bind_back.hpp>
#include <snap/functional/bind_front.hpp>
#include <snap/functional/invoke_r.hpp>
#include <snap/functional/is_reference_wrapper.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <vector>

TEST(FunctionalBind, BindFrontPrependsArguments)
{
	std::vector<int> observed;
	auto recorder =
		[&observed](int a, int b, int c)
		{
			observed = { a, b, c };
			return a + b + c;
		};

	auto bound = SNAP_NAMESPACE::bind_front(recorder, 1, 2);
	EXPECT_EQ(bound(3), 6);
	EXPECT_EQ(observed, (std::vector<int>{ 1, 2, 3 }));
}

TEST(FunctionalBind, BindFrontMovesBoundValuesOnce)
{
	auto ptr = std::make_unique<int>(7);
	auto bound =
		SNAP_NAMESPACE::bind_front([](std::unique_ptr<int> owned, int extra) { return *owned + extra; }, std::move(ptr));

	EXPECT_EQ(std::move(bound)(5), 12);
	EXPECT_EQ(ptr, nullptr) << "ownership should stay inside the binder";
}

TEST(FunctionalBind, BindBackAppendsArgumentsAndPreservesOrder)
{
	std::vector<int> observed;
	auto fn =
		[&observed](int a, int b, int c)
		{
			observed = { a, b, c };
			return a * 100 + b * 10 + c;
		};

	auto bound = SNAP_NAMESPACE::bind_back(fn, 2, 3);
	EXPECT_EQ(bound(1), 123);
	EXPECT_EQ(observed, (std::vector<int>{ 1, 2, 3 }));
}

TEST(FunctionalInvokeR, ForcesRequestedReturnType)
{
	auto make_double = [] { return 4.5; };
	EXPECT_EQ(SNAP_NAMESPACE::invoke_r<int>(make_double), 4);

	bool ran = false;
	SNAP_NAMESPACE::invoke_r<void>([&ran] { ran = true; });
	EXPECT_TRUE(ran);
}

TEST(FunctionalReferenceWrapperTrait, DetectsReferenceWrappers)
{
	static_assert(SNAP_NAMESPACE::is_reference_wrapper<std::reference_wrapper<int>>::value);
	static_assert(!SNAP_NAMESPACE::is_reference_wrapper<int>::value);
	SUCCEED();
}

