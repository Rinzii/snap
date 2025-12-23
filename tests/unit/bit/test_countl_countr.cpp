#include "snap/bit/countl.hpp"
#include "snap/bit/countr.hpp"
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

	template <class T> constexpr int reference_countl_zero(T value)
	{
		if (value == T{ 0 }) return digits_v<T>;
		int count = 0;
		for (int bit = digits_v<T> - 1; bit >= 0; --bit)
		{
			if ((value >> bit) & T{ 1 }) break;
			++count;
		}
		return count;
	}

	template <class T> constexpr int reference_countl_one(T value)
	{
		if (value == std::numeric_limits<T>::max()) return digits_v<T>;
		int count = 0;
		for (int bit = digits_v<T> - 1; bit >= 0; --bit)
		{
			if (((value >> bit) & T{ 1 }) == T{ 0 }) break;
			++count;
		}
		return count;
	}

	template <class T> constexpr int reference_countr_zero(T value)
	{
		if (value == T{ 0 }) return digits_v<T>;
		int count = 0;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			if ((value >> bit) & T{ 1 }) break;
			++count;
		}
		return count;
	}

	template <class T> constexpr int reference_countr_one(T value)
	{
		if (value == std::numeric_limits<T>::max()) return digits_v<T>;
		int count = 0;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			if (((value >> bit) & T{ 1 }) == T{ 0 }) break;
			++count;
		}
		return count;
	}

	template <class T> class CountLeadingTrailingTyped : public ::testing::Test
	{
	};

	using UnsignedTypes = ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;

	TYPED_TEST_SUITE(CountLeadingTrailingTyped, UnsignedTypes);

	TYPED_TEST(CountLeadingTrailingTyped, CountlZeroMatchesReference)
	{
		using T			 = TypeParam;
		const int digits = digits_v<T>;
		if constexpr (digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T value = T{ 0 };; value = T(value + T{ 1 }))
			{
				EXPECT_EQ(snap::countl_zero(value), reference_countl_zero(value)) << +value;
				if (value == max_value) break;
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = T{ 1 } << bit;
				EXPECT_EQ(snap::countl_zero(value), reference_countl_zero(value));
				EXPECT_EQ(snap::countl_zero(T(value - T{ 1 })), reference_countl_zero(T(value - T{ 1 })));
			}
		}
		EXPECT_EQ(snap::countl_zero(T{ 0 }), digits_v<T>);
		EXPECT_EQ(snap::countl_zero(std::numeric_limits<T>::max()), 0);
	}

	TYPED_TEST(CountLeadingTrailingTyped, CountlOneMatchesReference)
	{
		using T			 = TypeParam;
		const int digits = digits_v<T>;
		if constexpr (digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T value = T{ 0 };; value = T(value + T{ 1 }))
			{
				EXPECT_EQ(snap::countl_one(value), reference_countl_one(value)) << +value;
				if (value == max_value) break;
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = T{ 1 } << bit;
				EXPECT_EQ(snap::countl_one(value), reference_countl_one(value));
				EXPECT_EQ(snap::countl_one(T(value - T{ 1 })), reference_countl_one(T(value - T{ 1 })));
			}
			EXPECT_EQ(snap::countl_one(std::numeric_limits<T>::max()), digits_v<T>);
		}
	}

	TYPED_TEST(CountLeadingTrailingTyped, CountrZeroMatchesReference)
	{
		using T			 = TypeParam;
		const int digits = digits_v<T>;
		if constexpr (digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T value = T{ 0 };; value = T(value + T{ 1 }))
			{
				EXPECT_EQ(snap::countr_zero(value), reference_countr_zero(value)) << +value;
				if (value == max_value) break;
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = T{ 1 } << bit;
				EXPECT_EQ(snap::countr_zero(value), reference_countr_zero(value));
				EXPECT_EQ(snap::countr_zero(T(value - T{ 1 })), reference_countr_zero(T(value - T{ 1 })));
			}
		}
		EXPECT_EQ(snap::countr_zero(T{ 0 }), digits_v<T>);
		EXPECT_EQ(snap::countr_zero(T{ 1 }), 0);
	}

	TYPED_TEST(CountLeadingTrailingTyped, CountrOneMatchesReference)
	{
		using T			 = TypeParam;
		const int digits = digits_v<T>;
		if constexpr (digits <= 12)
		{
			const T max_value = std::numeric_limits<T>::max();
			for (T value = T{ 0 };; value = T(value + T{ 1 }))
			{
				EXPECT_EQ(snap::countr_one(value), reference_countr_one(value)) << +value;
				if (value == max_value) break;
			}
		}
		else
		{
			for (int bit = 0; bit < digits; ++bit)
			{
				const T value = T{ 1 } << bit;
				EXPECT_EQ(snap::countr_one(value), reference_countr_one(value));
				EXPECT_EQ(snap::countr_one(T(value - T{ 1 })), reference_countr_one(T(value - T{ 1 })));
			}
			EXPECT_EQ(snap::countr_one(std::numeric_limits<T>::max()), digits_v<T>);
		}
	}

#if SNAP_HAS_CPP20
	TYPED_TEST(CountLeadingTrailingTyped, StdBitConsistencyWhenAvailable)
	{
		using T = TypeParam;
		for (int bit = 0; bit < digits_v<T>; ++bit)
		{
			const T value = T{ 1 } << bit;
			EXPECT_EQ(snap::countl_zero(value), std::countl_zero(value));
			EXPECT_EQ(snap::countr_zero(value), std::countr_zero(value));
			EXPECT_EQ(snap::countl_one(value), std::countl_one(value));
			EXPECT_EQ(snap::countr_one(value), std::countr_one(value));
		}
	}
#endif

} // namespace
