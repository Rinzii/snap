#ifndef SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP
#define SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/builtin/type_traits/is_constant_evaluated.hpp"

SNAP_BEGIN_NAMESPACE
constexpr bool is_constant_evaluated() noexcept
{
	return SNAP_NAMESPACE::builtin::is_constant_evaluated();
}
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_TYPE_TRAITS_IS_CONSTANT_EVALUATED_HPP
