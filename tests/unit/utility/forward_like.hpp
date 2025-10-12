#pragma once

#include <type_traits>
#include <utility> // std::move, std::as_const

namespace snap
{

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

} // namespace snap
