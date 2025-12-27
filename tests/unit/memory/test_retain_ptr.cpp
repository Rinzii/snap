#include <gtest/gtest.h>

#include <snap/internal/abi_namespace.hpp>
#include <snap/memory/retain_ptr.hpp>

namespace
{
	struct RefCounted : SNAP_NAMESPACE::atomic_reference_count<RefCounted>
	{
		RefCounted(const RefCounted&)			 = delete;
		RefCounted& operator=(const RefCounted&) = delete;
		RefCounted(RefCounted&&)				 = delete;
		RefCounted& operator=(RefCounted&&)		 = delete;
		explicit RefCounted(int v) : value(v) { ++instances; }
		~RefCounted() { --instances; }

		int value;
		static inline int instances = 0;
	};
} // namespace

TEST(MemoryRetainPtr, ManagesReferenceCounts)
{
	EXPECT_EQ(RefCounted::instances, 0);
	{
		auto ptr = SNAP_NAMESPACE::make_retain<RefCounted>(5);
		EXPECT_TRUE(ptr);
		EXPECT_EQ(ptr.use_count(), 1);
		EXPECT_EQ(ptr->value, 5);

		{
			// NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
			const auto copy = ptr;
			EXPECT_EQ(ptr.use_count(), 2);
			EXPECT_EQ(copy.use_count(), 2);
		}

		EXPECT_EQ(ptr.use_count(), 1);
	}
	EXPECT_EQ(RefCounted::instances, 0);
}

TEST(MemoryRetainPtr, AdoptAndRetainBehaviors)
{
	auto* raw = new RefCounted(9); // NOLINT(cppcoreguidelines-owning-memory)
	{
		SNAP_NAMESPACE::retain_ptr<RefCounted> adopted(raw, SNAP_NAMESPACE::adopt_object);
		EXPECT_EQ(adopted.use_count(), 1);

		SNAP_NAMESPACE::retain_ptr<RefCounted> retained(raw, SNAP_NAMESPACE::retain_object);
		EXPECT_EQ(retained.use_count(), 2);
		EXPECT_EQ(adopted.use_count(), 2);
	}
	EXPECT_EQ(RefCounted::instances, 0);
}
