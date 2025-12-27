#include "snap/span.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

#if __cplusplus >= 202002L
#	include <span>
#endif

namespace
{

TEST(Span, DefaultConstructedDynamicExtentIsEmpty)
{
	SNAP_NAMESPACE::span<int> s;
	EXPECT_EQ(s.size(), 0U);
	EXPECT_EQ(s.data(), nullptr);
	EXPECT_TRUE(s.empty());
}

TEST(Span, StaticExtentFromArrayPreservesSizeAndData)
{
	int values[3] = { 1, 2, 3 };
	SNAP_NAMESPACE::span<int, 3> s(values);

	EXPECT_EQ(s.size(), 3U);
	EXPECT_EQ(s.front(), 1);
	EXPECT_EQ(s.back(), 3);
	EXPECT_EQ(&s[0], values);
}

TEST(Span, ContainerConstructorBindsToVector)
{
	std::vector<int> values = { 4, 5, 6, 7 };
	SNAP_NAMESPACE::span<int> s(values);

	ASSERT_EQ(s.size(), values.size());
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		EXPECT_EQ(s[i], values[i]);
	}
}

TEST(Span, SubspanAndFirstLastCreateExpectedViews)
{
	int values[5] = { 10, 20, 30, 40, 50 };
	SNAP_NAMESPACE::span<int> s(values);

	const auto head = s.first(2);
	ASSERT_EQ(head.size(), 2U);
	EXPECT_EQ(head.front(), 10);

	const auto tail = s.last(2);
	ASSERT_EQ(tail.size(), 2U);
	EXPECT_EQ(tail.back(), 50);

	const auto middle = s.subspan(1, 3);
	ASSERT_EQ(middle.size(), 3U);
	EXPECT_EQ(middle.front(), 20);
	EXPECT_EQ(middle.back(), 40);
}

TEST(Span, ConstConversionAndAsBytesAreValid)
{
	std::array<std::uint16_t, 2> data = { 0x0102U, 0x0304U };
	SNAP_NAMESPACE::span<std::uint16_t> writable(data);
	SNAP_NAMESPACE::span<const std::uint16_t> readable = writable;

	EXPECT_EQ(readable.data(), writable.data());

	const auto bytes = SNAP_NAMESPACE::as_bytes(readable);
	ASSERT_EQ(bytes.size(), readable.size_bytes());
	EXPECT_EQ(bytes.data(),
			  reinterpret_cast<const SNAP_NAMESPACE::byte*>(readable.data()));

	auto mutable_bytes = SNAP_NAMESPACE::as_writable_bytes(writable);
	ASSERT_EQ(mutable_bytes.size(), writable.size_bytes());
	mutable_bytes[0] = static_cast<SNAP_NAMESPACE::byte>(0xFF);
	EXPECT_EQ(reinterpret_cast<std::uint8_t*>(writable.data())[0], 0xFF);
}

TEST(Span, DeductionGuidesPickExpectedTypes)
{
	int values[4] = { 1, 2, 3, 4 };
	SNAP_NAMESPACE::span dyn(values, static_cast<std::size_t>(4));
	SNAP_NAMESPACE::span stat(values);

	static_assert(std::is_same_v<decltype(dyn), SNAP_NAMESPACE::span<int>>);
	static_assert(std::is_same_v<decltype(stat), SNAP_NAMESPACE::span<int, 4>>);
	EXPECT_EQ(dyn.size(), 4U);
	EXPECT_EQ(stat.size(), 4U);
}

using SNAP_NAMESPACE::get;

TEST(Span, StructuredGetReturnsReferences)
{
	std::array<int, 2> data = { 9, 8 };
	const SNAP_NAMESPACE::span<int, 2> s(data);

	int& first  = get<0>(s);
	int& second = get<1>(s);
	EXPECT_EQ(&first, data.data());
	EXPECT_EQ(&second, data.data() + 1);
}

#if __cplusplus >= 202002L

TEST(SpanStdParity, ConstructorsMirrorStdSpan)
{
	int values[] = { 1, 2, 3, 4 };

	SNAP_NAMESPACE::span   snap_dyn(values);
	std::span              std_dyn(values);
	EXPECT_EQ(snap_dyn.size(), std_dyn.size());
	EXPECT_EQ(snap_dyn.data(), std_dyn.data());

	SNAP_NAMESPACE::span<int, 4> snap_static(values);
	std::span<int, 4>            std_static(values);
	EXPECT_EQ(snap_static.size(), std_static.size());
	EXPECT_EQ(snap_static.data(), std_static.data());
}

TEST(SpanStdParity, SubspanOperationsMatchStdSpan)
{
	int values[] = { 10, 20, 30, 40, 50 };

	SNAP_NAMESPACE::span<int> snap(values);
	std::span<int>            std_span(values);

	const auto snap_first = snap.first(3);
	const auto std_first  = std_span.first(3);
	EXPECT_TRUE(std::equal(snap_first.begin(), snap_first.end(), std_first.begin()));

	const auto snap_last = snap.last(2);
	const auto std_last  = std_span.last(2);
	EXPECT_TRUE(std::equal(snap_last.begin(), snap_last.end(), std_last.begin()));

	const auto snap_mid = snap.subspan(1, 3);
	const auto std_mid  = std_span.subspan(1, 3);
	EXPECT_TRUE(std::equal(snap_mid.begin(), snap_mid.end(), std_mid.begin()));
}

TEST(SpanStdParity, ConversionToConstMatchesStdSpan)
{
	int values[] = { 7, 8, 9 };

	SNAP_NAMESPACE::span<int> snap_mut(values);
	std::span<int>            std_mut(values);

	SNAP_NAMESPACE::span<const int> snap_const = snap_mut;
	std::span<const int>            std_const = std_mut;

	EXPECT_TRUE(std::equal(snap_const.begin(), snap_const.end(), std_const.begin()));
}

TEST(SpanStdParity, IteratorParity)
{
	std::array<int, 3> values = { 1, 2, 3 };

	SNAP_NAMESPACE::span<int> snap(values);
	std::span<int>            std_span(values);

	EXPECT_TRUE(std::equal(snap.begin(), snap.end(), std_span.begin()));
	EXPECT_TRUE(std::equal(snap.rbegin(), snap.rend(), std_span.rbegin()));
}

TEST(SpanStdParity, DeductionGuidesMatchStdSpan)
{
	std::array<long, 2> array_values = { 4, 5 };
	std::vector<long>   vec_values   = { 6, 7, 8 };

	SNAP_NAMESPACE::span snap_from_arr(array_values);
	std::span            std_from_arr(array_values);
	static_assert(std::is_same_v<decltype(snap_from_arr), SNAP_NAMESPACE::span<long, 2>>);
	static_assert(std::is_same_v<decltype(std_from_arr), std::span<long, 2>>);

	SNAP_NAMESPACE::span snap_from_vec(vec_values);
	std::span            std_from_vec(vec_values);
	EXPECT_TRUE(std::equal(snap_from_vec.begin(), snap_from_vec.end(), std_from_vec.begin()));
}

TEST(SpanStdParity, MakeSpanMatchesStdSpan)
{
	int values[] = { 11, 12, 13 };

	const auto snap_made = SNAP_NAMESPACE::make_span(values);
	const auto std_made  = std::span(values);

	EXPECT_TRUE(std::equal(snap_made.begin(), snap_made.end(), std_made.begin()));
}

TEST(SpanStdParity, TupleInterfaceCompat)
{
	std::array<int, 2> values = { 5, 6 };
	SNAP_NAMESPACE::span<int, 2> snap(values);
	std::span<int, 2>            std_span(values);

	EXPECT_EQ(get<0>(snap), std_span[0]);
	EXPECT_EQ(get<1>(snap), std_span[1]);
}

TEST(SpanStdParity, BytesViewMatchesStdSpan)
{
	std::uint16_t values[] = { 0x0102U, 0x0304U };

	const auto snap_bytes = SNAP_NAMESPACE::as_bytes(SNAP_NAMESPACE::span(values));
	const auto std_bytes  = std::as_bytes(std::span(values));
	EXPECT_TRUE(std::equal(snap_bytes.begin(), snap_bytes.end(), std_bytes.begin()));
}

#endif // __cplusplus >= 202002L

} // namespace

