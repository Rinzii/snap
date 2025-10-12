#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <cmath>

#include "snap/bit/bit_cast.hpp"

// ----------------------------- byte helpers -----------------------------
template<class T>
static std::array<unsigned char, sizeof(T)> to_bytes(const T& v) {
  std::array<unsigned char, sizeof(T)> b{};
  std::memcpy(b.data(), &v, sizeof(T));
  return b;
}

template<class T>
static T from_bytes(std::array<unsigned char, sizeof(T)> b) {
  T v{};
  std::memcpy(&v, b.data(), sizeof(T));
  return v;
}

template<class T, std::size_t N>
constexpr bool arr_eq(const std::array<T,N>& a, const std::array<T,N>& b) {
  for (std::size_t i = 0; i < N; ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// Recognize std::array<unsigned char, N>
template<class T> struct is_uc_array : std::false_type {};
template<size_t N> struct is_uc_array<std::array<unsigned char, N>> : std::true_type {};
template<class T> constexpr bool is_uc_array_v = is_uc_array<T>::value;

// ----------------------------- samples -----------------------------
template<class T, class Enable = void>
struct Samples;

template<class T>
struct Samples<T, std::enable_if_t<std::is_integral_v<T>>> {
  static std::vector<T> values() {
    using U = std::make_unsigned_t<T>;
    return {
      static_cast<T>(0),
      static_cast<T>(1),
      static_cast<T>(~U{0}),
      static_cast<T>(U{0x55u}),
      static_cast<T>(U{0xAAu}),
      static_cast<T>(U{0x12345678u & ~U{0}}) // masked to width
    };
  }
};

template<class T>
struct Samples<T, std::enable_if_t<std::is_floating_point_v<T>>> {
  static std::vector<T> values() {
    std::vector<T> v = { T(0), T(-0.0), T(1), T(-1) };
    if (std::numeric_limits<T>::has_infinity) {
      v.push_back(std::numeric_limits<T>::infinity());
      v.push_back(-std::numeric_limits<T>::infinity());
    }
    if (std::numeric_limits<T>::has_quiet_NaN) {
      v.push_back(std::numeric_limits<T>::quiet_NaN());
    }
    if (std::numeric_limits<T>::denorm_min() > T(0)) {
      v.push_back(std::numeric_limits<T>::denorm_min());
    }
    return v;
  }
};

template<size_t N>
static std::vector<std::array<unsigned char, N>> array_patterns() {
  std::array<unsigned char, N> zeros{};
  std::array<unsigned char, N> ones{};
  std::memset(ones.data(), 0xFF, N);
  std::array<unsigned char, N> inc{};
  for (size_t i = 0; i < N; ++i) inc[i] = static_cast<unsigned char>(i);
  std::array<unsigned char, N> alt{};
  for (size_t i = 0; i < N; ++i) alt[i] = static_cast<unsigned char>(0xA5u ^ i);
  return { zeros, ones, inc, alt };
}

template<class T>
struct Samples<T, std::enable_if_t<is_uc_array_v<T>>> {
  static std::vector<T> values() {
    constexpr size_t N = sizeof(T);
    auto pats = array_patterns<N>();
    std::vector<T> out;
    out.reserve(pats.size());
    for (auto& p : pats) {
      T t{};
      std::memcpy(&t, p.data(), N);
      out.push_back(t);
    }
    return out;
  }
};

// A simple trivially-copyable struct with the same size as uint32_t on common ABIs
struct TwoU16 {
  std::uint16_t lo;
  std::uint16_t hi;
};
static_assert(std::is_trivially_copyable_v<TwoU16>, "expect trivially copyable");

template<>
struct Samples<TwoU16, void> {
  static std::vector<TwoU16> values() {
    return { TwoU16{0,0}, TwoU16{1,0xFFFFu}, TwoU16{0xA55Au,0x5AA5u}, TwoU16{0x1234u,0x5678u} };
  }
};

// ----------------------------- typed pairs -----------------------------
template<class To_, class From_> struct Pair { using To = To_; using From = From_; };

using BitCastPairs = ::testing::Types<
  Pair<std::uint32_t, float>,
  Pair<float, std::uint32_t>,
  Pair<std::uint64_t, double>,
  Pair<double, std::uint64_t>,
  Pair<std::array<unsigned char, 4>, std::uint32_t>,
  Pair<std::uint32_t, std::array<unsigned char, 4>>,
  Pair<std::array<unsigned char, 8>, std::uint64_t>,
  Pair<std::uint64_t, std::array<unsigned char, 8>>,
  Pair<std::uint32_t, TwoU16>,
  Pair<TwoU16, std::uint32_t>
>;

template<class P>
class BitCastTyped : public ::testing::Test {
protected:
  using To   = typename P::To;
  using From = typename P::From;
  static_assert(sizeof(To) == sizeof(From), "size mismatch for typed pair");
  static_assert(std::is_trivially_copyable_v<To>, "To must be trivially copyable");
  static_assert(std::is_trivially_copyable_v<From>, "From must be trivially copyable");
};
TYPED_TEST_SUITE(BitCastTyped, BitCastPairs);

// Round-trip preserves object representation (bytes) regardless of type
TYPED_TEST(BitCastTyped, RoundTripPreservesBytes) {
  using To   = typename TestFixture::To;
  using From = typename TestFixture::From;

  for (const auto& f : Samples<From>::values()) {
    To t = snap::bit_cast<To>(f);
    From back = snap::bit_cast<From>(t);
    EXPECT_EQ(to_bytes(back), to_bytes(f));
  }
}

// If either side is a byte array, the one-way cast must match its raw bytes
TYPED_TEST(BitCastTyped, ByteArrayExactMappingWhenPresent) {
  using To   = typename TestFixture::To;
  using From = typename TestFixture::From;

  if constexpr (is_uc_array_v<From>) {
    for (const auto& arr : Samples<From>::values()) {
      To t = snap::bit_cast<To>(arr);
      EXPECT_EQ(to_bytes(t), to_bytes(arr));
    }
  }
  if constexpr (is_uc_array_v<To>) {
    for (const auto& f : Samples<From>::values()) {
      To t = snap::bit_cast<To>(f);
      EXPECT_EQ(to_bytes(t), to_bytes(f));
    }
  }
}

// IEEE-754 specific checks (only if the platform says it's IEC 559)
TEST(BitCastFloatIEEE, NegativeZeroPreserved) {
  if constexpr (!std::numeric_limits<float>::is_iec559) GTEST_SKIP() << "Non-IEC559 float";
  constexpr std::uint32_t bits = 0x80000000u;
  const float f = snap::bit_cast<float>(bits);
  EXPECT_TRUE(std::signbit(f));
  EXPECT_EQ(snap::bit_cast<std::uint32_t>(f), bits);
}

TEST(BitCastFloatIEEE, InfinitiesAndNaNRoundTrip) {
  if constexpr (!std::numeric_limits<double>::is_iec559) GTEST_SKIP() << "Non-IEC559 double";
  constexpr double inf = std::numeric_limits<double>::infinity();
  constexpr double neg_inf = -std::numeric_limits<double>::infinity();
  constexpr double qnan = std::numeric_limits<double>::quiet_NaN();

  // We can only assert round-trip bit equality, not value equality (NaN != NaN).
  const auto b_inf  = snap::bit_cast<std::uint64_t>(inf);
  const auto b_neg_inf = snap::bit_cast<std::uint64_t>(neg_inf);
  const auto b_qnan = snap::bit_cast<std::uint64_t>(qnan);

  EXPECT_EQ(snap::bit_cast<double>(b_inf), inf);
  EXPECT_EQ(snap::bit_cast<double>(b_neg_inf), neg_inf);
  // For NaN, check NaN-ness and exact round-trip bits
  const double back = snap::bit_cast<double>(b_qnan);
  EXPECT_TRUE(std::isnan(back));
  EXPECT_EQ(snap::bit_cast<std::uint64_t>(back), b_qnan);
}

