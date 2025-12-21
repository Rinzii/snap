#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/internal/builtin/type_traits/is_constant_evaluated.hpp"

SNAP_BEGIN_NAMESPACE
constexpr bool is_constant_evaluated() noexcept
{
	return snap::builtin::is_constant_evaluated();
}
SNAP_END_NAMESPACE
