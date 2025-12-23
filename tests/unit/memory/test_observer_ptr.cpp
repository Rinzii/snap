#include <snap/internal/abi_namespace.hpp>

#include <snap/memory/observer_ptr.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <unordered_set>

namespace
{
	struct Base
	{
		virtual ~Base() = default;
	};

	struct Derived : Base
	{
	};
} // namespace

TEST(MemoryObserverPtr, BasicOperations)
{
	int value = 42;

	SNAP_NAMESPACE::observer_ptr<int> ptr(&value);
	EXPECT_TRUE(ptr);
	EXPECT_EQ(*ptr, 42);
	EXPECT_EQ(ptr.get(), &value);

	ptr.reset();
	EXPECT_FALSE(ptr);

	ptr.reset(&value);
	auto copy = ptr;
	EXPECT_EQ(copy.get(), &value);

	int* raw = copy.release();
	EXPECT_EQ(raw, &value);
	EXPECT_FALSE(copy);
}

TEST(MemoryObserverPtr, AllowsCovariantConversions)
{
	Derived			   derived;
	SNAP_NAMESPACE::observer_ptr derived_ptr(&derived);
	SNAP_NAMESPACE::observer_ptr<Base> base_ptr(derived_ptr);
	EXPECT_EQ(base_ptr.get(), static_cast<Base*>(&derived));

	EXPECT_TRUE(base_ptr == derived_ptr);
	EXPECT_FALSE(base_ptr == nullptr);
}

TEST(MemoryObserverPtr, HashUsesUnderlyingPointer)
{
	int						 value = 5;
	SNAP_NAMESPACE::observer_ptr<int> ptr(&value);
	std::unordered_set<SNAP_NAMESPACE::observer_ptr<int>> set;
	set.insert(ptr);
	EXPECT_EQ(set.count(ptr), 1);
}

TEST(MemoryObserverPtr, MakeObserverConstructsConveniently)
{
	int value = 9;
	auto ptr   = SNAP_NAMESPACE::make_observer(&value);
	EXPECT_TRUE(ptr);
	EXPECT_EQ(*ptr, 9);
}

