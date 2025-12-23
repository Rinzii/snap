#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/compat/constexpr.hpp"
#include "snap/memory/construct_at.hpp"
#include "snap/type_traits/is_bounded_array.hpp"

#include <cstddef>	   // std::byte, std::max_align_t
#include <memory>	   // std::addressof
#include <new>		   // placement new
#include <type_traits> // std::is_array_v, std::is_constructible_v
#include <utility>	   // std::forward, std::launder, std::addresso

SNAP_BEGIN_NAMESPACE
/*
 * inplace_storage
 * ---------------
 * Purpose: a small, fixed-size, properly aligned buffer to construct/destroy
 * exactly one live object or one bounded array in-place (no heap).
 *
 * Why this over std::aligned_storage?
 *  - aligned_storage is deprecated and easy to misuse.
 *  - provides m_engaged tracking, laundered typed access, array helpers, and
 *    compile-time fit checks in one place.
 *
 * Pros:
 *  - Safer, clearer API; active-lifetime tracking; array support; works with
 *    incomplete T via raw<T>().
 * Cons:
 *  - Caller must pass the active type to get()/destroy().
 *  - Only one live entity at a time; no visitation like std::variant.
 *  - No synchronization on m_engaged.
 */
template <std::size_t Size, std::size_t Alignment = alignof(std::max_align_t)> class inplace_storage
{
	static_assert(Size > 0, "Size must be > 0");
	static_assert(Alignment != 0, "Alignment must be non-zero");
	static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");

public:
	static constexpr std::size_t size	   = Size;
	static constexpr std::size_t alignment = Alignment;

	constexpr inplace_storage() noexcept = default;
	SNAP_CONSTEXPR20 ~inplace_storage()	 = default;

	inplace_storage(const inplace_storage&)			   = delete;
	inplace_storage& operator=(const inplace_storage&) = delete;
	inplace_storage(inplace_storage&&)				   = delete;
	inplace_storage& operator=(inplace_storage&&)	   = delete;

	[[nodiscard]] constexpr bool has_value() const noexcept { return m_engaged; }
	constexpr explicit operator bool() const noexcept { return m_engaged; }

	// Opaque view for placement-new
	[[nodiscard]] constexpr void* data() noexcept { return static_cast<void*>(std::addressof(m_storage[0])); }
	[[nodiscard]] constexpr const void* data() const noexcept { return static_cast<const void*>(std::addressof(m_storage[0])); }

	// Pre-construction typed addresses (don’t dereference before construct())
	template <typename T> [[nodiscard]] constexpr T* raw() noexcept { return static_cast<T*>(static_cast<void*>(std::addressof(m_storage[0]))); }
	template <typename T> [[nodiscard]] constexpr const T* raw() const noexcept
	{
		return static_cast<const T*>(static_cast<const void*>(std::addressof(m_storage[0])));
	}

	// Post-construction access (use launder after placement-new)
	template <typename T> [[nodiscard]] constexpr T* get() noexcept
	{
		static_assert(sizeof(T) <= Size, "T is too large for this inplace_storage");
		static_assert(alignof(T) <= Alignment, "T requires stricter alignment than this inplace_storage provides");
		return std::launder(static_cast<T*>(static_cast<void*>(std::addressof(m_storage[0]))));
	}
	template <typename T> [[nodiscard]] constexpr const T* get() const noexcept
	{
		static_assert(sizeof(T) <= Size, "T is too large for this inplace_storage");
		static_assert(alignof(T) <= Alignment, "T requires stricter alignment than this inplace_storage provides");
		return std::launder(static_cast<const T*>(static_cast<const void*>(std::addressof(m_storage[0]))));
	}

	// For constexpr tests not touching the buffer
	template <class T, class... Args> static SNAP_CONSTEVAL T construct_value(Args&&... args) { return T(std::forward<Args>(args)...); }

	// Construct T or T[N] in-place.
	template <typename T, typename... Args> constexpr T& construct(Args&&... args)
	{
		static_assert(sizeof(T) <= Size, "T is too large for this inplace_storage");
		static_assert(alignof(T) <= Alignment, "T requires stricter alignment than this inplace_storage provides");
		static_assert(!std::is_array_v<T> || sizeof...(Args) == 0, "construct<T[N]>() only supports value-initialization with no arguments");

		if constexpr (std::is_array_v<T>)
		{
			using E = std::remove_extent_t<T>;
			::new (data()) E[std::extent_v<T>]();
			m_engaged = true; // set only after success
			return *get<T>();
		}
		else
		{
			T* p	  = ::new (data()) T(std::forward<Args>(args)...);
			m_engaged = true; // set only after success
			return *p;
		}
	}

	template <typename T, typename... Args> constexpr T& emplace(Args&&... args) { return construct<T>(std::forward<Args>(args)...); }

	// Destroy the active object/array. Caller must provide the active T.
	template <typename T> constexpr void destruct()
	{
		if (!m_engaged) return;		  // idempotent
		::SNAP_NAMESPACE::destroy_at(get<T>()); // array-aware
		m_engaged = false;
	}
	template <typename T> constexpr void destroy() { destruct<T>(); }

	// Capacity query
	template <typename T> struct fits : std::bool_constant<(sizeof(T) <= Size && alignof(T) <= Alignment)>
	{
	};
	template <typename T> static constexpr bool fits_v = fits<T>::value;

private:
	alignas(Alignment) std::byte m_storage[Size];
	bool m_engaged = false;
};

SNAP_END_NAMESPACE
