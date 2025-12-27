#include <gtest/gtest.h>

#include <snap/internal/abi_namespace.hpp>
#include <snap/memory/inout_ptr.hpp>
#include <snap/memory/out_ptr.hpp>
#include <snap/memory/ptr_traits.hpp>

#include <memory>

namespace
{
	void AcquireRaw(int** target, int value)
	{
		*target = new int(value); // NOLINT(cppcoreguidelines-owning-memory)
	}
} // namespace

TEST(MemoryOutPtr, ResetsSmartPointerAfterCall)
{
	std::unique_ptr<int> ptr(new int(1));
	{
		auto adapter = SNAP_NAMESPACE::out_ptr(ptr);
		AcquireRaw(adapter, 99);
	}

	ASSERT_TRUE(ptr);
	EXPECT_EQ(*ptr, 99);
}

TEST(MemoryOutPtr, SupportsSharedPtrWithDeleterArgs)
{
	std::shared_ptr<int> ptr;
	{
		auto adapter = SNAP_NAMESPACE::out_ptr(ptr, std::default_delete<int>());
		AcquireRaw(adapter, 77);
	}

	ASSERT_TRUE(ptr);
	EXPECT_EQ(*ptr, 77);
}

TEST(MemoryInoutPtr, ReplacesManagedObject)
{
	std::unique_ptr<int> ptr(new int(5));
	int* old_raw = ptr.get(); // NOLINT(misc-const-correctness)
	{
		auto adapter = SNAP_NAMESPACE::inout_ptr(ptr);
		AcquireRaw(adapter, 42);
	}

	EXPECT_TRUE(ptr);
	EXPECT_EQ(*ptr, 42);
	EXPECT_NE(ptr.get(), old_raw);
	delete old_raw; // release() leaks the previous pointer by design NOLINT(cppcoreguidelines-owning-memory)
}

TEST(MemoryInoutPtr, WorksWithRawPointers)
{
	int* raw = nullptr; // NOLINT(misc-const-correctness)
	{
		auto adapter = SNAP_NAMESPACE::inout_ptr(raw);
		AcquireRaw(adapter, 12);
	}

	ASSERT_NE(raw, nullptr);
	EXPECT_EQ(*raw, 12);
	delete raw; // NOLINT(cppcoreguidelines-owning-memory)
}

TEST(MemoryPtrTraits, DetectsCapabilities)
{
	static_assert(SNAP_NAMESPACE::is_resettable_smart_pointer_v<std::unique_ptr<int>>);
	static_assert(!SNAP_NAMESPACE::is_resettable_smart_pointer_v<int>);
	static_assert(SNAP_NAMESPACE::is_releasable_smart_pointer_v<std::unique_ptr<int>>);
	static_assert(!SNAP_NAMESPACE::is_releasable_smart_pointer_v<std::shared_ptr<int>>);
}
