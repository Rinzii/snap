#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/always_false.hpp"

SNAP_BEGIN_NAMESPACE
struct placeholder_t
{
	placeholder_t() = default;

	template <class... Ts> constexpr placeholder_t(Ts &&...) noexcept {}

	template <class T> [[nodiscard]] operator T() const noexcept
	{
		static_assert(SNAP_NAMESPACE::always_false<T>, "conversion of SNAP_NAMESPACE::placeholder_t is ill-formed");
		return nullptr;
	}
};
SNAP_END_NAMESPACE
