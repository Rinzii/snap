#include "snap/expected/unexpected.hpp"
#include "snap/meta/detector.hpp"
#include "snap/numbers.hpp"
#include "snap/type_traits/remove_cvref.hpp"

#include <gtest/gtest.h>

#include <string>
#include <type_traits>
#include <vector>

namespace
{
	template <class T> using value_type_t = typename T::value_type;
}

TEST(Expected, UnexpectedConstructsAndSwaps)
{
	SNAP_NAMESPACE::unexpected<int> numeric(std::in_place, 7);
	SNAP_NAMESPACE::unexpected<int> copy = numeric;
	EXPECT_EQ(numeric.error(), 7);
	EXPECT_EQ(copy.error(), 7);

	SNAP_NAMESPACE::unexpected<std::string> message(std::in_place, "oops");
	EXPECT_EQ(message.error(), "oops");

	SNAP_NAMESPACE::unexpected<std::string> other(std::in_place, "other");
	swap(message, other);
	EXPECT_EQ(message.error(), "other");
	EXPECT_EQ(other.error(), "oops");
}

TEST(Meta, DetectorIdentifiesValueType)
{
	static_assert(SNAP_NAMESPACE::is_detected_v<value_type_t, std::vector<int>>);
	static_assert(!SNAP_NAMESPACE::is_detected_v<value_type_t, int>);

	using detected = SNAP_NAMESPACE::detected_t<value_type_t, std::vector<int>>;
	static_assert(std::is_same_v<detected, int>);

	using fallback = SNAP_NAMESPACE::detected_or_t<double, value_type_t, int>;
	static_assert(std::is_same_v<fallback, double>);

	SUCCEED();
}

TEST(Numbers, ProvideConstants)
{
	EXPECT_DOUBLE_EQ(SNAP_NAMESPACE::numbers::pi, SNAP_NAMESPACE::numbers::pi_v<double>);
	EXPECT_DOUBLE_EQ(SNAP_NAMESPACE::numbers::e, SNAP_NAMESPACE::numbers::e_v<double>);
	EXPECT_FLOAT_EQ(SNAP_NAMESPACE::numbers::pi_v<float>, static_cast<float>(SNAP_NAMESPACE::numbers::pi));
}

TEST(TypeTraits, RemoveCvrefRemovesQualifiers)
{
	static_assert(std::is_same_v<SNAP_NAMESPACE::remove_cvref_t<const int &>, int>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::remove_cvref_t<volatile long &&>, long>);
	SUCCEED();
}
