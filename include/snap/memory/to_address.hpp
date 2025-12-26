#ifndef SNP_INCLUDE_SNAP_MEMORY_TO_ADDRESS_HPP
#define SNP_INCLUDE_SNAP_MEMORY_TO_ADDRESS_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <memory>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
namespace detail
{
	// Detects: std::pointer_traits<T>::to_address(std::declval<const T&>())
	template <class T, class = void> struct has_ptr_traits_to_address : std::false_type
	{
	};

	template <class T> struct has_ptr_traits_to_address<T, std::void_t<decltype(std::pointer_traits<T>::to_address(std::declval<const T&>()))>> : std::true_type
	{
	};

	template <class T> inline constexpr bool has_ptr_traits_to_address_v = has_ptr_traits_to_address<T>::value;
} // namespace detail

// Raw pointers
template <class T> constexpr T* to_address(T* p) noexcept
{
	static_assert(!std::is_function_v<T>, "to_address cannot be used with function types");
	return p;
}

// Pointer-like types (e.g. smart/fancy pointers)
template <class T> constexpr auto to_address(const T& p) noexcept
{
	if constexpr (detail::has_ptr_traits_to_address_v<T>) { return std::pointer_traits<T>::to_address(p); }
	else
	{
		// Recurse through operator-> until we hit the raw-pointer overload
		return ::SNAP_NAMESPACE::to_address(p.operator->());
	}
}

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_MEMORY_TO_ADDRESS_HPP
