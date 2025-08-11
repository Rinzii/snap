// tests/unit/bit/test_bit_ceil.cpp
#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "snap/bit/bit_ceil.hpp"
#include "snap/internal/compat/std.hpp"

// If available, pull in std::bit_ceil to cross-check
#if SNAP_HAS_CPP20
  #include <bit>
#endif

namespace {

template <class T>
constexpr bool is_power_of_two(T v) { return v != T{0} && (v & (v - T{1})) == T{0}; }

template <class T>
constexpr int digits_v = std::numeric_limits<T>::digits; // value bits

template <class T>
constexpr T highest_pow2() { return T{1} << (digits_v<T> - 1); }

// Typed test over common unsigned integer types
template <class T, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
class BitCeilTyped : public ::testing::Test { };

using TestTypes = ::testing::Types<
  unsigned char,
  unsigned short,
  unsigned int,
  unsigned long,
  unsigned long long
>;
TYPED_TEST_SUITE(BitCeilTyped, TestTypes);

// 0 and 1 well-defined
TYPED_TEST(BitCeilTyped, ZeroAndOne) {
  using T = TypeParam;
  EXPECT_EQ(snap::bit_ceil(T{0}), T{1});
  EXPECT_EQ(snap::bit_ceil(T{1}), T{1});
}

// Powers of two up to the highest representable are fixed points
TYPED_TEST(BitCeilTyped, PowersOfTwoAreFixedPoints) {
  using T = TypeParam;
  for (int k = 0; k < digits_v<T>; ++k) {
    const T p = T{1} << k;
    EXPECT_EQ(snap::bit_ceil(p), p) << "k=" << k;
  }
}

// Edge: for k==1, m = 2^1 - 1 == 1, and bit_ceil(1) == 1 (not 2)
TYPED_TEST(BitCeilTyped, JustBelow_k1_EdgeCase) {
  using T = TypeParam;
  const T m = T{1};
  EXPECT_EQ(snap::bit_ceil(m), T{1});
}

// Just below a power-of-two rounds up (true for k >= 2)
TYPED_TEST(BitCeilTyped, JustBelowRoundsUp_kGe2) {
  using T = TypeParam;
  for (int k = 2; k < digits_v<T>; ++k) {
    const T p = T{1} << k;      // 2^k
    const T m = T(p - T{1});    // 2^k - 1
    EXPECT_EQ(snap::bit_ceil(m), p) << "k=" << k;
  }
}

// Just above a power-of-two rounds up to next power (while defined)
TYPED_TEST(BitCeilTyped, JustAboveRoundsUpWithinRange) {
  using T = TypeParam;
  for (int k = 0; k < digits_v<T> - 1; ++k) {
    const T p = T{1} << k;            // 2^k
    const T a = T(p + T{1});          // 2^k + 1
    const T expect = T{1} << (k + 1); // 2^(k+1)
    EXPECT_EQ(snap::bit_ceil(a), expect) << "k=" << k;
  }
}

// Structural properties within [0, highest_pow2]
TYPED_TEST(BitCeilTyped, BoundsAndPowerOfTwoProperty) {
  using T = TypeParam;
  const T hp2 = highest_pow2<T>();
  const int digits = digits_v<T>;

  auto check_props = [&](T x) {
    const T r = snap::bit_ceil(x);
    if (x == T{0}) { EXPECT_EQ(r, T{1}); return; }
    EXPECT_TRUE((r & (r - T{1})) == 0);   // power of two
    EXPECT_LE(x, r);
    if (r != T{1}) EXPECT_GT(x, T(r >> 1));
  };

  if constexpr (digits <= 16) {
    // Exhaustive for small types
    for (T x = T{0}; x <= hp2; x = T(x + T{1})) check_props(x);
  } else {
    // Sampled for large types
    check_props(T{0});
    check_props(T{1});

    for (int k = 0; k < digits; ++k) {
      const T p = T{1} << k;             // 2^k
      if (p > hp2) break;
      check_props(p);                    // exact power-of-two is fixed point

      if (k > 0) {
        const T prev = T{1} << (k - 1);  // 2^(k-1)
        check_props(T(prev + T{1}));     // just above prev -> p
        check_props(T(p - T{1}));        // just below p    -> p
      }

      if (k + 1 < digits) {
        check_props(T(p + T{1}));        // just above p -> 2^(k+1) (still defined)
      }
    }

    // Extra density near the top boundary (last 256 values, if available)
    if (hp2 > T{256}) {
      for (T x = T(hp2 - T{256}); x <= hp2; x = T(x + T{1})) check_props(x);
    }
  }

  // Spot checks near the boundary
  if (hp2 > T{1}) EXPECT_EQ(snap::bit_ceil(T(hp2 - T{1})), hp2);
  EXPECT_EQ(snap::bit_ceil(hp2), hp2);
}

  // Monotonic on defined domain (sampled; single increasing pass)
  TYPED_TEST(BitCeilTyped, MonotonicOnDefinedDomain) {
  using T = TypeParam;
  const T hp2 = highest_pow2<T>();
  const int digits = digits_v<T>;

  auto ceilT = [](T x) -> T { return snap::bit_ceil(x); };

  std::vector<T> xs;
  xs.reserve(40000); // upper bound; stays small/fast

  if constexpr (digits <= 16) {
    // Exhaustive for small types
    for (T x = T{1}; x <= hp2; x = T(x + T{1})) xs.push_back(x);
  } else {
    // Start with the lowest defined point
    xs.push_back(T{1});

    // Add boundary neighborhoods in increasing order: {p-1, p, p+1}
    for (int k = 1; k < digits; ++k) {
      const T p = T{1} << k;
      if (p > hp2) break;

      const T below = T(p - T{1});
      if (below >= xs.back()) xs.push_back(below); // monotonically increasing
      if (p > xs.back())       xs.push_back(p);

      if (k + 1 < digits) {
        const T above = T(p + T{1});
        if (above <= hp2 && above > xs.back()) xs.push_back(above);
      }
    }

    // Coarse sweep to fill gaps (~2^15 total steps max)
    int shift = 0;
    if (digits > 15) {
      shift = digits - 15;
    }
    const T step = (shift > 0) ? T{1} << shift : T{1};

    for (T x = T{1}; x < hp2; ) {
      T nx = T(x + step);
      if (nx <= x) break;
      if (nx > xs.back() && nx <= hp2) xs.push_back(nx);
      if (nx > hp2) break;
      x = nx;
    }

    if (hp2 > xs.back()) xs.push_back(hp2);
  }

  // Single monotonic pass
  T prev = ceilT(T{0}); // bit_ceil(0) == 1
  for (T x : xs) {
    const T cur = ceilT(x);
    EXPECT_LE(prev, cur) << "x=" << +x;
    prev = cur;
  }
}


// Cheap exhaustive for uint8_t within defined domain
TEST(BitCeilExhaustive, UInt8_DefinedDomain) {
  using T = std::uint8_t;
  const T hp2 = highest_pow2<T>(); // 128
  for (unsigned v = 0; v <= static_cast<unsigned>(hp2); ++v) {
    const T x = static_cast<T>(v);
    const T r = snap::bit_ceil(x);
    if (x == T{0}) {
      EXPECT_EQ(r, T{1});
    } else {
      EXPECT_TRUE((r & (r - T{1})) == 0);
      EXPECT_LE(x, r);
      if (r != T{1}) EXPECT_GT(x, T(r >> 1));
    }
  }
}

// Compile-time (well-defined inputs only)
static_assert(snap::bit_ceil(std::uint32_t{0}) == 1u);
static_assert(snap::bit_ceil(std::uint32_t{1}) == 1u);
static_assert(snap::bit_ceil(std::uint32_t{2}) == 2u);
static_assert(snap::bit_ceil(std::uint32_t{3}) == 4u);
static_assert(snap::bit_ceil(std::uint32_t{4}) == 4u);
static_assert(snap::bit_ceil(std::uint32_t{5}) == 8u);
static_assert(snap::bit_ceil(std::uint32_t{0x80000000u}) == 0x80000000u);
// Do not constexpr-test inputs above highest power-of-two; that’s UB by spec.

// Cross-check against std::bit_ceil (sampled for wide types)
#if SNAP_HAS_CPP20
TYPED_TEST(BitCeilTyped, MatchesStdBitCeilWithinDefinedDomain) {
  using T = TypeParam;
  const int digits = digits_v<T>;
  const T hp2 = highest_pow2<T>();

  auto check = [&](T x) {
    EXPECT_EQ(snap::bit_ceil(x), std::bit_ceil(x)) << +x;
  };

  if constexpr (digits <= 16) {
    for (T x = T{0}; x <= hp2; x = T(x + T{1})) check(x);
  } else {
    check(T{0});
    check(T{1});
    for (int k = 0; k < digits; ++k) {
      const T p = T{1} << k;
      if (p > hp2) break;
      if (k > 0) check(T(p - T{1}));     // just below p
      check(p);                          // at p
      if (k + 1 < digits) check(T(p + T{1})); // just above p (still defined)
    }
    if (hp2 > T{256}) {
      for (T x = T(hp2 - T{256}); x <= hp2; x = T(x + T{1})) check(x);
    }
  }
}
#endif

} // namespace
