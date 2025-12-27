#include "snap/bit/bit_floor.hpp"
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

	template <class T> class BitFloorTyped : public ::testing::Test
	{
	};

	SNAP_TYPED_TEST_SUITE(BitFloorTyped, SNAP_NAMESPACE::test::type_sets::CommonUnsigned);

	TYPED_TEST(BitFloorTyped, ZeroIsZero)
	{
		using T = TypeParam;
		EXPECT_EQ(SNAP_NAMESPACE::bit_floor(T{ 0 }), T{ 0 });
	}

	TYPED_TEST(BitFloorTyped, PowersOfTwoAreFixedPoints)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = pow2<T>(bit);
			EXPECT_EQ(SNAP_NAMESPACE::bit_floor(value), value) << "bit=" << bit;
		}
	}

	TYPED_TEST(BitFloorTyped, ValuesBetweenPowersRoundDown)
	{
		using T = TypeParam;
		for (int bit = 1; bit < digits_v<T>; ++bit)
		{
			const T high = pow2<T>(bit);		 // 2^bit
			const T low	 = pow2<T>(bit - 1); // 2^(bit-1)
			const T mid	 = T(low + (high - low) / 2);
			EXPECT_EQ(SNAP_NAMESPACE::bit_floor(T(high - T{ 1 })), low) << "bit=" << bit;
			EXPECT_EQ(SNAP_NAMESPACE::bit_floor(mid), low) << "bit=" << bit;
		}
	}

	TYPED_TEST(BitFloorTyped, MaxValue)
	{
		using T			  = TypeParam;
		const T max_value = std::numeric_limits<T>::max();
		const T expected  = pow2<T>(digits_v<T> - 1);
		EXPECT_EQ(SNAP_NAMESPACE::bit_floor(max_value), expected);
	}

#if SNAP_HAS_CPP20
	TYPED_TEST(BitFloorTyped, MatchesStdBitFloorWhenAvailable)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = pow2<T>(bit);
			EXPECT_EQ(SNAP_NAMESPACE::bit_floor(value), std::bit_floor(value));
			if (bit > 0)
			{
				const T prev = pow2<T>(bit - 1);
				const T value_minus_one = static_cast<T>(value - T{ 1 });
				const T prev_plus_one	= static_cast<T>(prev + T{ 1 });
				EXPECT_EQ(SNAP_NAMESPACE::bit_floor(value_minus_one), std::bit_floor(value_minus_one));
				EXPECT_EQ(SNAP_NAMESPACE::bit_floor(prev_plus_one), std::bit_floor(prev_plus_one));
			}
		}
	}
#endif

} // namespace
