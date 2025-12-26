#ifndef SNP_INCLUDE_SNAP_META_PLACEHOLDER_HPP
#define SNP_INCLUDE_SNAP_META_PLACEHOLDER_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/always_false.hpp"

SNAP_BEGIN_NAMESPACE
struct placeholder_t
{
	placeholder_t() = default;

	template <class... Ts> explicit constexpr placeholder_t(Ts &&...) noexcept {}

	template <class T> [[nodiscard]] explicit operator T() const noexcept
	{
		static_assert(SNAP_NAMESPACE::always_false<T>, "conversion of SNAP_NAMESPACE::placeholder_t is ill-formed");
		return nullptr;
	}
};
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_META_PLACEHOLDER_HPP
