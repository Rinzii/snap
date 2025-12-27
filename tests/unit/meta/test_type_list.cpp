#include <gtest/gtest.h>

#include <snap/meta/type_list.hpp>

#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace
{
	template <class... Ts> struct capture_pack
	{
		static constexpr std::size_t size = sizeof...(Ts);
	};
} // namespace

TEST(TypeList, ApplyPropagatesPack)
{
	using list	  = SNAP_NAMESPACE::type_list<int, double, char>;
	using applied = SNAP_NAMESPACE::typelist::apply_t<list, capture_pack>;
	using expect  = capture_pack<int, double, char>;
	static_assert(std::is_same_v<applied, expect>);
	EXPECT_EQ(applied::size, 3U);
}

TEST(TypeList, FromExtractsStdTypes)
{
	using tup_list = SNAP_NAMESPACE::typelist::from_tuple_t<std::tuple<int, long, float>>;
	static_assert(std::is_same_v<tup_list, SNAP_NAMESPACE::type_list<int, long, float>>);

	using var_list = SNAP_NAMESPACE::typelist::from_variant_t<std::variant<int, double>>;
	static_assert(std::is_same_v<var_list, SNAP_NAMESPACE::type_list<int, double>>);

	using pair_list = SNAP_NAMESPACE::typelist::from_pair_t<std::pair<int, double>>;
	static_assert(std::is_same_v<pair_list, SNAP_NAMESPACE::type_list<int, double>>);
}

TEST(TypeList, RoundTripsThroughStdContainers)
{
	using list = SNAP_NAMESPACE::type_list<int, short>;

	using tuple_t = SNAP_NAMESPACE::typelist::to_tuple_t<list>;
	static_assert(std::is_same_v<tuple_t, std::tuple<int, short>>);

	using variant_t = SNAP_NAMESPACE::typelist::to_variant_t<list>;
	static_assert(std::is_same_v<variant_t, std::variant<int, short>>);

	using pair_t = SNAP_NAMESPACE::typelist::to_pair_t<list>;
	static_assert(std::is_same_v<pair_t, std::pair<int, short>>);

	using tuple_back   = SNAP_NAMESPACE::typelist::from_tuple_t<tuple_t>;
	using variant_back = SNAP_NAMESPACE::typelist::from_variant_t<variant_t>;
	using pair_back	   = SNAP_NAMESPACE::typelist::from_pair_t<pair_t>;

	static_assert(std::is_same_v<tuple_back, list>);
	static_assert(std::is_same_v<variant_back, list>);
	static_assert(std::is_same_v<pair_back, list>);
}
