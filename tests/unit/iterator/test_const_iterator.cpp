#include <gtest/gtest.h>

#include <snap/internal/abi_namespace.hpp>
#include <snap/iterator/const_iterator.hpp>
#include <snap/iterator/is_contiguous_iterator.hpp>

#include <array>
#include <list>
#include <type_traits>
#include <vector>

TEST(IteratorConstIterator, WrapsRawIteratorAndEnforcesConstAccess)
{
	const std::vector<int> values{ 1, 2, 3 };

	auto it = SNAP_NAMESPACE::make_const_iterator(values.begin());
	static_assert(std::is_same_v<decltype(*it), const int &>);
	EXPECT_EQ(*it, 1);

	auto copy(it);
	EXPECT_EQ(copy, it);

	auto raw = values.begin();
	EXPECT_TRUE(it == raw);
	EXPECT_TRUE(raw == it);
}

TEST(IteratorConstIterator, SupportsRandomAccessOperations)
{
	std::array<int, 4> arr{ 1, 2, 3, 4 };
	auto begin = SNAP_NAMESPACE::make_const_iterator(arr.begin());
	auto end   = SNAP_NAMESPACE::make_const_iterator(arr.end());

	EXPECT_EQ(end - begin, 4);
	EXPECT_EQ(begin[2], 3);

	auto advanced = begin + 3;
	EXPECT_EQ(*advanced, 4);
	EXPECT_EQ(advanced - begin, 3);

	auto rewound = advanced - 2;
	EXPECT_EQ(*rewound, 2);
}

TEST(IteratorConstIterator, ProducesConstSentinelForIteratorLikeTypes)
{
	const std::vector<int> data{ 10, 20 };
	auto sentinel = SNAP_NAMESPACE::make_const_sentinel(data.cend());
	auto iter	  = SNAP_NAMESPACE::make_const_iterator(data.cbegin());

	EXPECT_FALSE(iter == sentinel);
	iter += 2;
	EXPECT_TRUE(iter == sentinel);
}

TEST(IteratorConcepts, DetectsContiguousIterators)
{
	static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<int *>);
	static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<const int *>);
	// TODO: Figure out why this is failing.
	// static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<std::vector<int>::iterator>);
	// static_assert(SNAP_NAMESPACE::is_contiguous_iterator_v<std::vector<int>::const_iterator>);
	static_assert(!SNAP_NAMESPACE::is_contiguous_iterator_v<std::list<int>::iterator>);
}
