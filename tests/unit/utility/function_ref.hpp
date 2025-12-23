#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
namespace details
{
	template <class Sig> struct signature_qualifiers;

	template <class R, class... Args> struct signature_qualifiers<R(Args...)>
	{
		using function					  = R(Args...);
		static constexpr bool is_noexcept = false;

		template <class... T> static constexpr bool is_invocable_using = std::is_invocable_r_v<R, T..., Args...>;

		template <class T> using cv = T;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) noexcept>
	{
		using function					  = R(Args...);
		static constexpr bool is_noexcept = true;

		template <class... T> static constexpr bool is_invocable_using = std::is_nothrow_invocable_r_v<R, T..., Args...>;

		template <class T> using cv = T;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) const> : signature_qualifiers<R(Args...)>
	{
		template <class T> using cv = T const;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) const noexcept> : signature_qualifiers<R(Args...) noexcept>
	{
		template <class T> using cv = T const;
	};
} // namespace details
SNAP_END_NAMESPACE
