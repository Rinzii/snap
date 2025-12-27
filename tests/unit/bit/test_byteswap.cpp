#include "snap/bit/byteswap.hpp"

#include "snap/testing/gtest_helpers.hpp"

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <vector>

namespace
{

	template <class T> constexpr T reference_byteswap(T value)
	{
		using U					 = std::make_unsigned_t<T>;
		constexpr int byte_count = static_cast<int>(sizeof(T));
		const U u				 = static_cast<U>(value);
		U result				 = 0;
		for (int i = 0; i < byte_count; ++i)
		{
			const U byte = (u >> (i * 8)) & static_cast<U>(0xFFU);
			result |= byte << ((byte_count - 1 - i) * 8);
		}
		return static_cast<T>(result);
	}

	template <class T> class ByteswapTyped : public ::testing::Test
	{
	};

	using IntegralTypes = ::testing::Types<std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, std::uint64_t, std::int64_t>;

	SNAP_TYPED_TEST_SUITE(ByteswapTyped, IntegralTypes);

	TYPED_TEST(ByteswapTyped, MatchesReferenceAndIsInvolutory)
	{
		using T = TypeParam;
		std::vector<T> samples;
		samples.push_back(T{ 0 });
		samples.push_back(T{ 1 });
		samples.push_back(static_cast<T>(-1));
		samples.push_back(std::numeric_limits<T>::max());
		samples.push_back(std::numeric_limits<T>::min());
		samples.push_back(static_cast<T>(0x0102030405060708ULL));

		for (const T value : samples)
		{
			const T swapped = SNAP_NAMESPACE::byteswap(value);
			EXPECT_EQ(swapped, reference_byteswap(value)) << +value;
			EXPECT_EQ(SNAP_NAMESPACE::byteswap(swapped), value);
		}
	}

	TEST(Byteswap, DistinguishesBytePatterns)
	{
		constexpr std::uint32_t value = 0x01020304U;
		const auto swapped			  = SNAP_NAMESPACE::byteswap(value);
		EXPECT_NE(swapped, value);
		EXPECT_EQ(swapped, 0x04030201U);
	}

} // namespace
