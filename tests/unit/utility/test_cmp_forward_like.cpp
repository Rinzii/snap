#include <gtest/gtest.h>

#include <snap/internal/abi_namespace.hpp>
#include <snap/utility/cmp.hpp>
#include <snap/utility/forward_like.hpp>
#include <snap/utility/nontype.hpp>
#include <snap/utility/unreachable.hpp>

#include <type_traits>
#include <utility>

namespace
{
	template <class T, class U, class = void> struct cmp_equal_is_valid : std::false_type
	{
	};

	template <class T, class U> struct cmp_equal_is_valid<T, U, std::void_t<decltype(SNAP_NAMESPACE::cmp_equal(std::declval<T>(), std::declval<U>()))>>
		: std::true_type
	{
	};
} // namespace

TEST(UtilityCmp, HandlesSignedUnsignedMix)
{
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_equal(5, 5u));
	EXPECT_FALSE(SNAP_NAMESPACE::cmp_equal(-1, 1u));
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_less(-1, 2u)) << "negative signed values should compare as less";
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_greater(10u, -1));
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_less_equal(42, 42u));
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_greater_equal(0u, 0));
	EXPECT_TRUE(SNAP_NAMESPACE::cmp_not_equal(1, 2u));

	static_assert(!cmp_equal_is_valid<bool, bool>::value);
}

TEST(UtilityForwardLike, MirrorsReferenceQualifiers)
{
	int value		 = 7;
	const int cvalue = 11;

	static_assert(std::is_lvalue_reference_v<decltype(SNAP_NAMESPACE::forward_like<int&>(value))>);
	static_assert(std::is_same_v<decltype(SNAP_NAMESPACE::forward_like<const int&>(value)), const int&>);
	static_assert(std::is_same_v<decltype(SNAP_NAMESPACE::forward_like<int&&>(std::move(value))), int&&>);
	static_assert(std::is_same_v<decltype(SNAP_NAMESPACE::forward_like<const int&&>(std::move(value))), const int&&>);
	static_assert(std::is_same_v<decltype(SNAP_NAMESPACE::forward_like<const int&>(cvalue)), const int&>);

	auto&& forwarded = SNAP_NAMESPACE::forward_like<int&&>(std::move(value));
	EXPECT_EQ(&forwarded, &value);
}

TEST(UtilityNontype, ExposesDistinctTypePerValue)
{
	constexpr auto tag = SNAP_NAMESPACE::nontype<42>;
	(void)tag;

	static_assert(std::is_same_v<decltype(tag), const SNAP_NAMESPACE::nontype_t<42>>);
	static_assert(!std::is_same_v<SNAP_NAMESPACE::nontype_t<1>, SNAP_NAMESPACE::nontype_t<2>>);
}

#if GTEST_HAS_DEATH_TEST
TEST(UtilityUnreachable, TriggersDeath)
{
	EXPECT_DEATH(SNAP_NAMESPACE::unreachable(), "");
}
#else
TEST(UtilityUnreachable, TriggersDeath)
{
	GTEST_SKIP() << "Death tests not supported on this platform.";
}
#endif
