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
	snap::unexpected<int> numeric(std::in_place, 7);
	snap::unexpected<int> copy = numeric;
	EXPECT_EQ(numeric.error(), 7);
	EXPECT_EQ(copy.error(), 7);

	snap::unexpected<std::string> message(std::in_place, "oops");
	EXPECT_EQ(message.error(), "oops");

	snap::unexpected<std::string> other(std::in_place, "other");
	swap(message, other);
	EXPECT_EQ(message.error(), "other");
	EXPECT_EQ(other.error(), "oops");
}

TEST(Meta, DetectorIdentifiesValueType)
{
	static_assert(snap::is_detected_v<value_type_t, std::vector<int>>);
	static_assert(!snap::is_detected_v<value_type_t, int>);

	using detected = snap::detected_t<value_type_t, std::vector<int>>;
	static_assert(std::is_same_v<detected, int>);

	using fallback = snap::detected_or_t<double, value_type_t, int>;
	static_assert(std::is_same_v<fallback, double>);

	SUCCEED();
}

TEST(Numbers, ProvideConstants)
{
	EXPECT_DOUBLE_EQ(snap::numbers::pi, snap::numbers::pi_v<double>);
	EXPECT_DOUBLE_EQ(snap::numbers::e, snap::numbers::e_v<double>);
	EXPECT_FLOAT_EQ(snap::numbers::pi_v<float>, static_cast<float>(snap::numbers::pi));
}

TEST(TypeTraits, RemoveCvrefRemovesQualifiers)
{
	static_assert(std::is_same_v<snap::remove_cvref_t<const int &>, int>);
	static_assert(std::is_same_v<snap::remove_cvref_t<volatile long &&>, long>);
	SUCCEED();
}
