#include <gtest/gtest.h>

#include "snap/utility/in_range.hpp"

#include <cstdint>
#include <limits>

namespace
{

constexpr std::int64_t kBelowInt32Min = static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()) - 1;
constexpr std::int64_t kAboveInt32Max = static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()) + 1;
constexpr std::uint64_t kAboveUint32Max = static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1;

static_assert(snap::in_range<std::int32_t>(std::int64_t{0}), "0 should be representable as int32_t");
static_assert(snap::in_range<std::int32_t>(std::numeric_limits<std::int32_t>::min()), "min should be representable");
static_assert(snap::in_range<std::int32_t>(std::numeric_limits<std::int32_t>::max()), "max should be representable");
static_assert(!snap::in_range<std::int32_t>(kBelowInt32Min), "below min should not be representable");
static_assert(!snap::in_range<std::int32_t>(kAboveInt32Max), "above max should not be representable");

static_assert(snap::in_range<std::uint32_t>(std::uint64_t{0}), "0 should be representable as uint32_t");
static_assert(snap::in_range<std::uint32_t>(std::numeric_limits<std::uint32_t>::max()), "max should be representable");
static_assert(!snap::in_range<std::uint32_t>(kAboveUint32Max), "above max should not be representable");
static_assert(!snap::in_range<std::uint32_t>(std::int64_t{-1}), "negative values cannot be uint32_t");

static_assert(snap::in_range<std::uint16_t>(std::uint32_t{std::numeric_limits<std::uint16_t>::max()}),
              "narrower unsigned max should fit");
static_assert(!snap::in_range<std::uint16_t>(std::uint32_t{std::numeric_limits<std::uint16_t>::max()} + 1),
              "narrower unsigned overflow should be rejected");

} // namespace

TEST(UtilityInRange, SignedBoundaryChecks)
{
        EXPECT_TRUE(snap::in_range<std::int32_t>(std::int64_t{-42}));
        EXPECT_TRUE(snap::in_range<std::int32_t>(std::numeric_limits<std::int32_t>::min()));
        EXPECT_TRUE(snap::in_range<std::int32_t>(std::numeric_limits<std::int32_t>::max()));
        EXPECT_FALSE(snap::in_range<std::int32_t>(kBelowInt32Min));
        EXPECT_FALSE(snap::in_range<std::int32_t>(kAboveInt32Max));
}

TEST(UtilityInRange, UnsignedBoundaryChecks)
{
        EXPECT_TRUE(snap::in_range<std::uint32_t>(std::uint64_t{1024}));
        EXPECT_TRUE(snap::in_range<std::uint32_t>(std::numeric_limits<std::uint32_t>::max()));
        EXPECT_FALSE(snap::in_range<std::uint32_t>(kAboveUint32Max));
        EXPECT_FALSE(snap::in_range<std::uint32_t>(std::int64_t{-1}));
}

TEST(UtilityInRange, MixedSignedUnsigned)
{
        EXPECT_TRUE(snap::in_range<std::int32_t>(std::uint32_t{42}));
        EXPECT_FALSE(snap::in_range<std::int32_t>(std::uint64_t{std::numeric_limits<std::int32_t>::max()} + 1));
        EXPECT_FALSE(snap::in_range<std::uint16_t>(std::int32_t{-10}));
}
