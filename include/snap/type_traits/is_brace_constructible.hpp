#ifndef SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_BRACE_CONSTRUCTIBLE_HPP
#define SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_BRACE_CONSTRUCTIBLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
// C++17: detect brace-constructibility (aggregate-friendly)
template <class T, class... Args> struct is_brace_constructible
{
private:
	template <class U, class... A> static auto test(int) -> decltype(U{ std::declval<A>()... }, std::true_type{});
	template <class...> static auto test(...) -> std::false_type;

public:
	static constexpr bool value = decltype(test<T, Args...>(0))::value;
};

template <class T, class... Args> inline constexpr bool is_brace_constructible_v = is_brace_constructible<T, Args...>::value;
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_BRACE_CONSTRUCTIBLE_HPP
