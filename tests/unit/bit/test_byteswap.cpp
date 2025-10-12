#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "snap/bit/byteswap.hpp"

namespace {

template <class T>
constexpr T reference_byteswap(T value)
{
  using U = std::make_unsigned_t<T>;
  constexpr int byte_count = static_cast<int>(sizeof(T));
  U u = static_cast<U>(value);
  U result = 0;
  for (int i = 0; i < byte_count; ++i) {
    const U byte = (u >> (i * 8)) & static_cast<U>(0xFFu);
    result |= byte << ((byte_count - 1 - i) * 8);
  }
  return static_cast<T>(result);
}

template <class T>
class ByteswapTyped : public ::testing::Test { };

using IntegralTypes = ::testing::Types<
  std::uint8_t,
  std::int8_t,
  std::uint16_t,
  std::int16_t,
  std::uint32_t,
  std::int32_t,
  std::uint64_t,
  std::int64_t
>;

TYPED_TEST_SUITE(ByteswapTyped, IntegralTypes);

TYPED_TEST(ByteswapTyped, MatchesReferenceAndIsInvolutory)
{
  using T = TypeParam;
  std::vector<T> samples;
  samples.push_back(T{0});
  samples.push_back(T{1});
  samples.push_back(static_cast<T>(-1));
  samples.push_back(std::numeric_limits<T>::max());
  samples.push_back(std::numeric_limits<T>::min());
  samples.push_back(static_cast<T>(0x0102030405060708ull));

  for (T value : samples) {
    const T swapped = snap::byteswap(value);
    EXPECT_EQ(swapped, reference_byteswap(value)) << +value;
    EXPECT_EQ(snap::byteswap(swapped), value);
  }
}

TEST(Byteswap, DistinguishesBytePatterns)
{
  constexpr std::uint32_t value = 0x01020304u;
  const auto swapped = snap::byteswap(value);
  EXPECT_NE(swapped, value);
  EXPECT_EQ(swapped, 0x04030201u);
}

} // namespace
