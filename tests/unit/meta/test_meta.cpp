#include <snap/internal/abi_namespace.hpp>

#include <snap/meta/always_false.hpp>
#include <snap/meta/has.hpp>
#include <snap/meta/placeholder.hpp>

#include <gtest/gtest.h>

#include <string>
#include <type_traits>
#include <vector>

template <class T> using begin_expr = decltype(std::declval<T&>().begin());
template <class T> using size_expr	= decltype(std::declval<T&>().size());
template <class T, class U> using equals_expr = decltype(std::declval<T>() == std::declval<U>());

TEST(MetaTraits, AlwaysFalseIsAlwaysFalse)
{
	static_assert(!SNAP_NAMESPACE::always_false<int>);
	static_assert(!SNAP_NAMESPACE::always_false<>);
	SUCCEED();
}

TEST(MetaTraits, PlaceholderToleratesAnyConstruction)
{
	SNAP_NAMESPACE::placeholder_t placeholder(42, "snap");
	(void)placeholder;
	static_assert(std::is_default_constructible_v<SNAP_NAMESPACE::placeholder_t>);
}

TEST(MetaTraits, HasUtilitiesDetectExpressions)
{
	static_assert(SNAP_NAMESPACE::has<begin_expr, std::vector<int>>);
	static_assert(!SNAP_NAMESPACE::has<begin_expr, int>);

	static_assert(SNAP_NAMESPACE::has_as<size_expr, std::size_t, std::vector<int>>);
	static_assert(!SNAP_NAMESPACE::has_as<size_expr, std::size_t, int>);

	static_assert(SNAP_NAMESPACE::has_either<equals_expr, int, double>);
	static_assert(!SNAP_NAMESPACE::has_either<equals_expr, std::string, SNAP_NAMESPACE::placeholder_t>);

	static_assert(SNAP_NAMESPACE::has_either_as<equals_expr, bool, int, double>);
	static_assert(!SNAP_NAMESPACE::has_either_as<equals_expr, bool, std::string, SNAP_NAMESPACE::placeholder_t>);
}

