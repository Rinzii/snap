#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class Enum> constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
	static_assert(std::is_enum_v<Enum>, "to_underlying requires an enum type");
	return static_cast<std::underlying_type_t<Enum>>(e);
}

SNAP_END_NAMESPACE
