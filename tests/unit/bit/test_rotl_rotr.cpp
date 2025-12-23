#include "snap/bit/rotl.hpp"
#include "snap/bit/rotr.hpp"
#include "snap/internal/compat/std.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>
#include <vector>

#if SNAP_HAS_CPP20
	#include <bit>
#endif

namespace
{

	template <class T> constexpr T reference_rotl(T value, int count)
	{
		const unsigned digits = static_cast<unsigned>(std::numeric_limits<T>::digits);
		if (digits == 0) return value;
		const unsigned shift = static_cast<unsigned>(count % static_cast<int>(digits) + static_cast<int>(digits)) % digits;
		if (shift == 0) return value;
		return static_cast<T>((value << shift) | (value >> (digits - shift)));
	}

	template <class T> constexpr T reference_rotr(T value, int count)
	{
		const unsigned digits = static_cast<unsigned>(std::numeric_limits<T>::digits);
		if (digits == 0) return value;
		const unsigned shift = static_cast<unsigned>(count % static_cast<int>(digits) + static_cast<int>(digits)) % digits;
		if (shift == 0) return value;
		return static_cast<T>((value >> shift) | (value << (digits - shift)));
	}

	template <class T> std::vector<T> sample_values()
	{
		using U				  = std::make_unsigned_t<T>;
		const unsigned digits = static_cast<unsigned>(std::numeric_limits<T>::digits);
		const U mask		  = digits == sizeof(U) * 8 ? static_cast<U>(~U{ 0 }) : ((U{ 1 } << digits) - U{ 1 });

		auto clip = [&](unsigned long long v) -> T { return static_cast<T>(static_cast<U>(v) & mask); };

		std::vector<T> values = {
			clip(0ull), clip(1ull), clip(0x5A5Au), clip(0x0102030405060708ull), clip(0x9E3779B97F4A7C15ull), static_cast<T>(std::numeric_limits<U>::max())
		};

		if (digits > 0)
		{
			values.push_back(clip(1ull << (digits - 1))); // highest bit set
		}
		return values;
	}

	template <class T> class RotateTyped : public ::testing::Test
	{
	};

	using UnsignedTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

	TYPED_TEST_SUITE(RotateTyped, UnsignedTypes);

	TYPED_TEST(RotateTyped, RotlMatchesReference)
	{
		using T			   = TypeParam;
		const auto samples = sample_values<T>();
		const int counts[] = { 0, 1, 2, -1, -2, 7, -7, 8, -8, 16, -16, 31, -31, 64, -64 };

		for (T value : samples)
		{
			for (int count : counts)
			{
				const T expected = reference_rotl(value, count);
				EXPECT_EQ(SNAP_NAMESPACE::rotl(value, count), expected) << +value << ", count=" << count;
			}
		}
	}

	TYPED_TEST(RotateTyped, RotrMatchesReference)
	{
		using T			   = TypeParam;
		const auto samples = sample_values<T>();
		const int counts[] = { 0, 1, 2, -1, -2, 7, -7, 8, -8, 16, -16, 31, -31, 64, -64 };

		for (T value : samples)
		{
			for (int count : counts)
			{
				const T expected = reference_rotr(value, count);
				EXPECT_EQ(SNAP_NAMESPACE::rotr(value, count), expected) << +value << ", count=" << count;
			}
		}
	}

	TYPED_TEST(RotateTyped, RotlAndRotrAreInverses)
	{
		using T			   = TypeParam;
		const int digits   = std::numeric_limits<T>::digits;
		const auto samples = sample_values<T>();
		for (int count = -digits * 2; count <= digits * 2; ++count)
		{
			for (T value : samples)
			{
				const T rotated = SNAP_NAMESPACE::rotl(value, count);
				EXPECT_EQ(SNAP_NAMESPACE::rotr(rotated, count), value);
			}
		}
	}

#if SNAP_HAS_CPP20
	TYPED_TEST(RotateTyped, StdRotateConsistency)
	{
		using T = TypeParam;
		for (int count = -4; count <= 4; ++count)
		{
			const T value = static_cast<T>(0x1234567890ABCDEFull);
			EXPECT_EQ(SNAP_NAMESPACE::rotl(value, count), std::rotl(value, count));
			EXPECT_EQ(SNAP_NAMESPACE::rotr(value, count), std::rotr(value, count));
		}
	}
#endif

} // namespace
