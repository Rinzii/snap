#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

#include "snap/bit/has_single_bit.hpp"

namespace {

template <class T>
class HasSingleBitTyped : public ::testing::Test { };

using UnsignedTypes = ::testing::Types<
  unsigned short,
  unsigned int,
  unsigned long,
  unsigned long long
>;


TYPED_TEST_SUITE(HasSingleBitTyped, UnsignedTypes);

TYPED_TEST(HasSingleBitTyped, ZeroIsFalse)
{
  using T = TypeParam;
  EXPECT_FALSE(snap::has_single_bit(T{0}));
}

TYPED_TEST(HasSingleBitTyped, PowersOfTwoAreTrue)
{
  using T = TypeParam;
  for (int bit = 0; bit < std::numeric_limits<T>::digits; ++bit) {
    const T value = T{1} << bit;
    EXPECT_TRUE(snap::has_single_bit(value));
  }
}

TYPED_TEST(HasSingleBitTyped, NonPowerOfTwoIsFalse)
{
  using T = TypeParam;
  for (int bit = 1; bit < std::numeric_limits<T>::digits; ++bit) {
    const T value = (T{1} << bit) | T{1};
    EXPECT_FALSE(snap::has_single_bit(value));
  }
}

TYPED_TEST(HasSingleBitTyped, MaxValueIsFalseUnlessSingleBit)
{
  using T = TypeParam;
  if constexpr (std::numeric_limits<T>::digits == 1) {
    EXPECT_TRUE(snap::has_single_bit(std::numeric_limits<T>::max()));
  } else {
    EXPECT_FALSE(snap::has_single_bit(std::numeric_limits<T>::max()));
  }
}

} // namespace
