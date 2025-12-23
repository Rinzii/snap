#include "snap/bit/bit_width.hpp"
#include "snap/internal/compat/std.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

#if SNAP_HAS_CPP20
	#include <bit>
#endif

namespace
{

	template <class T> constexpr int digits_v = std::numeric_limits<T>::digits;

	template <class T> constexpr int reference_bit_width(T value)
	{
		if (value == T{ 0 }) return 0;
		int width = 0;
		while (value != T{ 0 })
		{
			++width;
			value = static_cast<T>(value >> 1);
		}
		return width;
	}

	template <class T> class BitWidthTyped : public ::testing::Test
	{
	};

	using UnsignedTypes = ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;

	TYPED_TEST_SUITE(BitWidthTyped, UnsignedTypes);

	TYPED_TEST(BitWidthTyped, ZeroHasZeroWidth)
	{
		using T = TypeParam;
		EXPECT_EQ(snap::bit_width(T{ 0 }), 0);
	}

	TYPED_TEST(BitWidthTyped, PowersOfTwo)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = T{ 1 } << bit;
			EXPECT_EQ(snap::bit_width(value), bit + 1);
		}
	}

	TYPED_TEST(BitWidthTyped, MatchesReferenceImplementation)
	{
		using T			 = TypeParam;
		const int digits = digits_v<T>;
		if constexpr (digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T x = T{ 0 };; x = T(x + T{ 1 }))
			{
				EXPECT_EQ(snap::bit_width(x), reference_bit_width(x)) << +x;
				if (x == max_value) break;
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = T{ 1 } << bit;
				EXPECT_EQ(snap::bit_width(value), reference_bit_width(value));
				if (bit > 0)
				{
					const T prev = T{ 1 } << (bit - 1);
					EXPECT_EQ(snap::bit_width(T(value - T{ 1 })), reference_bit_width(T(value - T{ 1 })));
					EXPECT_EQ(snap::bit_width(T(prev + T{ 1 })), reference_bit_width(T(prev + T{ 1 })));
				}
			}

			// Additional random-looking samples by stepping through value space
			T sample = T{ 1 };
			for (int step = 0; step < digits * 4; ++step)
			{
				EXPECT_EQ(snap::bit_width(sample), reference_bit_width(sample));
				sample = T(sample + (sample >> 1) + T{ 1 });
				if (sample == T{ 0 }) break;
			}
		}
	}

	TYPED_TEST(BitWidthTyped, MonotonicIncrease)
	{
		using T			 = TypeParam;
		T prev_width	 = 0;
		const int digits = digits_v<T>;
		for (int bit = 0; bit < digits; ++bit)
		{
			const T value	= T{ 1 } << bit;
			const int width = snap::bit_width(value);
			EXPECT_GE(width, prev_width);
			prev_width = width;
		}
	}

#if SNAP_HAS_CPP20
	TYPED_TEST(BitWidthTyped, MatchesStdBitWidthWhenAvailable)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = T{ 1 } << bit;
			EXPECT_EQ(snap::bit_width(value), std::bit_width(value));
			if (bit > 0)
			{
				const T prev = T{ 1 } << (bit - 1);
				EXPECT_EQ(snap::bit_width(T(value - T{ 1 })), std::bit_width(T(value - T{ 1 })));
				EXPECT_EQ(snap::bit_width(T(prev + T{ 1 })), std::bit_width(T(prev + T{ 1 })));
			}
		}
	}
#endif

} // namespace
