#pragma once

#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <auto V> struct nontype_t
	{
		explicit nontype_t() = default;
	};
	template <auto V> inline constexpr nontype_t<V> nontype{};
SNAP_END_NAMESPACE
