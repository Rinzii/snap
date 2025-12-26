#ifndef SNP_INCLUDE_SNAP_FUNCTIONAL_IDENTITY_HPP
#define SNP_INCLUDE_SNAP_FUNCTIONAL_IDENTITY_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <functional>

SNAP_BEGIN_NAMESPACE
struct identity
{
	template <class T> [[nodiscard]] constexpr T&& operator()(T&& t) const noexcept { return std::forward<T>(t); }
	using is_transparent = void;
};

template <class T> struct is_identity : std::is_same<std::remove_cv_t<T>, identity>
{
};

template <class U> struct is_identity<std::reference_wrapper<U>> : std::is_same<std::remove_cv_t<U>, identity>
{
};

template <class T> inline constexpr bool is_identity_v = is_identity<T>::value;
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_FUNCTIONAL_IDENTITY_HPP
