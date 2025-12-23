#include <snap/internal/abi_namespace.hpp>

#include <snap/memory/construct_at.hpp>
#include <snap/memory/inplace_storage.hpp>
#include <snap/memory/raw_ptr.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <new>
#include <type_traits>

namespace
{
	struct Tracker
	{
		explicit Tracker(int v) : value(v) { ++instances; }
		Tracker(const Tracker&) = delete;
		Tracker& operator=(const Tracker&) = delete;
		~Tracker() { --instances; }

		int				   value;
		static inline int instances = 0;
	};
} // namespace

static_assert(std::is_same_v<SNAP_NAMESPACE::raw_ptr<int>, int*>);

TEST(MemoryConstructAt, BuildsAndDestroysObjects)
{
	alignas(Tracker) std::uint8_t storage[sizeof(Tracker)];
	auto*			   ptr = reinterpret_cast<Tracker*>(storage);

	auto* constructed = SNAP_NAMESPACE::construct_at(ptr, 7);
	EXPECT_EQ(constructed->value, 7);
	EXPECT_EQ(Tracker::instances, 1);

	SNAP_NAMESPACE::destroy_at(constructed);
	EXPECT_EQ(Tracker::instances, 0);
}

TEST(MemoryConstructAt, ValueInitializesArrays)
{
	alignas(int[2]) std::uint8_t storage[sizeof(int[2])];
	auto*			   arr = reinterpret_cast<int (*)[2]>(storage);

	auto* constructed = SNAP_NAMESPACE::construct_at<int[2]>(arr);
	EXPECT_EQ((*constructed)[0], 0);
	EXPECT_EQ((*constructed)[1], 0);

	SNAP_NAMESPACE::destroy_at(constructed);
}

TEST(MemoryInplaceStorage, ManagesLifetime)
{
	SNAP_NAMESPACE::inplace_storage<sizeof(Tracker), alignof(Tracker)> storage;
	EXPECT_FALSE(storage.has_value());

	auto& value = storage.construct<Tracker>(99);
	EXPECT_TRUE(storage.has_value());
	EXPECT_EQ(value.value, 99);

	storage.destroy<Tracker>();
	EXPECT_FALSE(storage.has_value());
	EXPECT_EQ(Tracker::instances, 0);
}

TEST(MemoryInplaceStorage, SupportsArrays)
{
	SNAP_NAMESPACE::inplace_storage<sizeof(int[3]), alignof(int[3])> storage;
	auto& arr = storage.construct<int[3]>();
	arr[0]	 = 1;
	arr[1]	 = 2;
	arr[2]	 = 3;

	auto* typed = storage.get<int[3]>();
	EXPECT_EQ((*typed)[1], 2);
	storage.destroy<int[3]>();
}

