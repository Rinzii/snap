#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

#include "snap/bit/bit_floor.hpp"
#include "snap/internal/compat/std.hpp"

#if SNAP_HAS_CPP20
  #include <bit>
#endif

namespace {

template <class T>
constexpr int digits_v = std::numeric_limits<T>::digits;

template <class T>
class BitFloorTyped : public ::testing::Test { };

using UnsignedTypes = ::testing::Types<
  unsigned char,
  unsigned short,
  unsigned int,
  unsigned long,
  unsigned long long
>;

TYPED_TEST_SUITE(BitFloorTyped, UnsignedTypes);

TYPED_TEST(BitFloorTyped, ZeroIsZero)
{
  using T = TypeParam;
  EXPECT_EQ(snap::bit_floor(T{0}), T{0});
}

TYPED_TEST(BitFloorTyped, PowersOfTwoAreFixedPoints)
{
  using T = TypeParam;
  for (int bit = 0; bit < digits_v<T>; ++bit) {
    const T value = T{1} << bit;
    EXPECT_EQ(snap::bit_floor(value), value) << "bit=" << bit;
  }
}

TYPED_TEST(BitFloorTyped, ValuesBetweenPowersRoundDown)
{
  using T = TypeParam;
  for (int bit = 1; bit < digits_v<T>; ++bit) {
    const T high = T{1} << bit;        // 2^bit
    const T low  = T{1} << (bit - 1);  // 2^(bit-1)
    const T mid  = T(low + (high - low) / 2);
    EXPECT_EQ(snap::bit_floor(T(high - T{1})), low) << "bit=" << bit;
    EXPECT_EQ(snap::bit_floor(mid), low) << "bit=" << bit;
  }
}

TYPED_TEST(BitFloorTyped, MaxValue)
{
  using T = TypeParam;
  const T max_value = std::numeric_limits<T>::max();
  const T expected  = T{1} << (digits_v<T> - 1);
  EXPECT_EQ(snap::bit_floor(max_value), expected);
}

#if SNAP_HAS_CPP20
TYPED_TEST(BitFloorTyped, MatchesStdBitFloorWhenAvailable)
{
  using T = TypeParam;
  for (int bit = 0; bit < digits_v<T>; ++bit) {
    const T value = T{1} << bit;
    EXPECT_EQ(snap::bit_floor(value), std::bit_floor(value));
    if (bit > 0) {
      const T prev = T{1} << (bit - 1);
      EXPECT_EQ(snap::bit_floor(T(value - T{1})), std::bit_floor(T(value - T{1})));
      EXPECT_EQ(snap::bit_floor(T(prev + T{1})), std::bit_floor(T(prev + T{1})));
    }
  }
}
#endif

} // namespace
