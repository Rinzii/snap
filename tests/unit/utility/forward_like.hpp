#ifndef SNP_TESTS_UNIT_UTILITY_FORWARD_LIKE_HPP
#define SNP_TESTS_UNIT_UTILITY_FORWARD_LIKE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>
#include <utility> // std::move, std::as_const

SNAP_BEGIN_NAMESPACE
template <class T, class U> constexpr decltype(auto) forward_like(U&& x) noexcept
{
	using T_no_ref			 = std::remove_reference_t<T>;
	constexpr bool add_const = std::is_const_v<T_no_ref>;

	if constexpr (std::is_lvalue_reference_v<T&&>)
	{
		if constexpr (add_const)
		{
			return std::as_const(x); // const lvalue
		}
		else
		{
			return static_cast<std::remove_reference_t<U>&>(x); // non-const lvalue
		}
	}
	else
	{
		if constexpr (add_const)
		{
			return std::move(std::as_const(x)); // const rvalue
		}
		else
		{
			return std::move(x); // non-const rvalue
		}
	}
}

SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_UTILITY_FORWARD_LIKE_HPP
