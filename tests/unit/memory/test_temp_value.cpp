#include <snap/internal/abi_namespace.hpp>

#include <snap/memory/temp_value.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <type_traits>

namespace
{
	template <class T> struct CountingAllocator
	{
		using value_type = T;

		int* constructs = nullptr;
		int* destroys   = nullptr;

		explicit CountingAllocator(int* c, int* d) : constructs(c), destroys(d) {}

		template <class U> CountingAllocator(const CountingAllocator<U>& other) : constructs(other.constructs), destroys(other.destroys) {}

		T* allocate(std::size_t n) { return std::allocator<T>{}.allocate(n); }
		void deallocate(T* p, std::size_t n) { std::allocator<T>{}.deallocate(p, n); }

		template <class U, class... Args> void construct(U* p, Args&&... args)
		{
			++(*constructs);
			::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
		}

		template <class U> void destroy(U* p)
		{
			++(*destroys);
			p->~U();
		}

		template <class U> struct rebind
		{
			using other = CountingAllocator<U>;
		};
	};
} // namespace

TEST(MemoryTempValue, UsesAllocatorForLifetime)
{
	int constructs = 0;
	int destroys   = 0;

	CountingAllocator<int> alloc(&constructs, &destroys);
	{
		SNAP_NAMESPACE::temp_value<int, CountingAllocator<int>> tmp(alloc, 17);
		EXPECT_EQ(tmp.get(), 17);
		tmp.get() = 99;
		EXPECT_EQ(tmp.get(), 99);
	}

	EXPECT_EQ(constructs, 1);
	EXPECT_EQ(destroys, 1);
}

