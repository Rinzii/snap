#include "snap/bit/popcount.hpp"
#include "snap/internal/compat/std.hpp"

#include "snap/testing/gtest_helpers.hpp"

#include <limits>
#include <type_traits>

#if SNAP_HAS_CPP20
	#include <bit>
#endif

namespace
{
	using ::SNAP_NAMESPACE::test::digits_v;
	using ::SNAP_NAMESPACE::test::pow2;

	template <class T> constexpr unsigned reference_popcount(T v) noexcept
	{
		static_assert(std::is_integral_v<T>, "popcount<T>: T must be an integral type");

		using U = std::make_unsigned_t<T>;
		U x		= static_cast<U>(v);

		unsigned c = 0;
		while (x)
		{
			x &= (x - 1); // clear lowest set bit
			++c;
		}
		return c;
	}

	template <class T> class PopcountTyped : public ::testing::Test
	{
	};

	SNAP_TYPED_TEST_SUITE(PopcountTyped, SNAP_NAMESPACE::test::type_sets::CommonUnsigned);

	// NOLINTNEXTLINE(readability-function-cognitive-complexity)
	TYPED_TEST(PopcountTyped, MatchesReference)
	{
		using T = TypeParam;
		if constexpr (const int digits = digits_v<T>; digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T value = T{ 0 };; value = T(value + T{ 1 }))
			{
				EXPECT_EQ(SNAP_NAMESPACE::popcount(value), reference_popcount(value)) << +value;
				if (value == max_value) { break; }
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = pow2<T>(bit);
				EXPECT_EQ(SNAP_NAMESPACE::popcount(value), reference_popcount(value));
				EXPECT_EQ(SNAP_NAMESPACE::popcount(T(value - T{ 1 })), reference_popcount(T(value - T{ 1 })));
			}
			EXPECT_EQ(SNAP_NAMESPACE::popcount(std::numeric_limits<T>::max()), digits_v<T>);
			EXPECT_EQ(SNAP_NAMESPACE::popcount(T{ 0 }), 0);
		}
	}

#if SNAP_HAS_CPP20
	TYPED_TEST(PopcountTyped, StdPopcountConsistency)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = pow2<T>(bit);
			EXPECT_EQ(SNAP_NAMESPACE::popcount(value), std::popcount(value));
		}
	}
#endif
} // namespace
