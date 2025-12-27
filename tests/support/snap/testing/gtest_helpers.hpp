#ifndef SNP_TESTS_SUPPORT_SNAP_TESTING_GTEST_HELPERS_HPP
#define SNP_TESTS_SUPPORT_SNAP_TESTING_GTEST_HELPERS_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/testing/type_name.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace test
{

template <class T> inline constexpr int digits_v = std::numeric_limits<T>::digits;

template <class T> constexpr T pow2(int bit)
{
	return static_cast<T>(T{ 1 } << bit);
}

template <class T> constexpr T highest_pow2()
{
	return pow2<T>(digits_v<T> - 1);
}

template <class T> constexpr bool is_power_of_two(T value)
{
	return value != T{ 0 } && (value & (value - T{ 1 })) == T{ 0 };
}

namespace type_sets
{

using CommonUnsigned = ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;
using UnsignedWithoutChar = ::testing::Types<unsigned short, unsigned int, unsigned long, unsigned long long>;
using FixedWidthUnsigned = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

} // namespace type_sets

template <class... Ts> using type_pack = ::testing::Types<Ts...>;

} // namespace test
SNAP_END_NAMESPACE

#define SNAP_TYPED_TEST_SUITE(SuiteName, Types) \
	TYPED_TEST_SUITE(SuiteName, Types, SNAP_NAMESPACE::test::TypeNameGenerator)

#endif // SNP_TESTS_SUPPORT_SNAP_TESTING_GTEST_HELPERS_HPP

