#include <snap/internal/abi_namespace.hpp>

#include <snap/iterator/is_contiguous_iterator.hpp>
#include <snap/memory/to_address.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <iterator>
#include <list>

namespace
{
	struct TagIterator
	{
		using value_type		= int;
		using difference_type	= std::ptrdiff_t;
		using pointer			= int*;
		using reference			= int&;
		using iterator_category = std::random_access_iterator_tag;
		using iterator_concept	= SNAP_NAMESPACE::contiguous_iterator_tag;

		int* ptr = nullptr;

		TagIterator() = default;
		explicit TagIterator(int* p) : ptr(p) {}

		int& operator*() const { return *ptr; }
		TagIterator& operator++()
		{
			++ptr;
			return *this;
		}
		TagIterator operator+(difference_type n) const { return TagIterator(ptr + n); }
		auto operator->() const { return ptr; }
		friend bool operator==(const TagIterator& a, const TagIterator& b) { return a.ptr == b.ptr; }
		friend bool operator!=(const TagIterator& a, const TagIterator& b) { return !(a == b); }
	};

	struct Proxy
	{
		int value;
	};

	struct NonLvalueRandomAccess
	{
		using value_type		= int;
		using difference_type	= std::ptrdiff_t;
		using iterator_category = std::random_access_iterator_tag;

		int* ptr = nullptr;

		Proxy operator*() const { return Proxy{ *ptr }; }
		NonLvalueRandomAccess& operator++()
		{
			++ptr;
			return *this;
		}
		auto operator->() const { return ptr; }
		friend bool operator==(const NonLvalueRandomAccess& a, const NonLvalueRandomAccess& b) { return a.ptr == b.ptr; }
		friend bool operator!=(const NonLvalueRandomAccess& a, const NonLvalueRandomAccess& b) { return !(a == b); }
	};
} // namespace

TEST(IteratorIsContiguous, RecognizesPointers)
{
	static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<int*>);
	static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<const int*>);
}

TEST(IteratorIsContiguous, RejectsNonContiguousStandardIterators)
{
	static_assert(!SNAP_NAMESPACE::is_contiguous_iterator_v<std::list<int>::iterator>);
}

TEST(IteratorIsContiguous, HonorsIteratorConceptTagAndReferenceRules)
{
	static_assert(SNAP_NAMESPACE::details::iter_concept_is_contiguous_v<TagIterator>);
	static_assert(!SNAP_NAMESPACE::is_contiguous_iterator_v<NonLvalueRandomAccess>);
}


