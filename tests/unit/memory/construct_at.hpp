#pragma once

#include "snap/internal/compat/constexpr.hpp"
#include "snap/type_traits/is_bounded_array.hpp"

#include <cassert>	   // assert
#include <memory>	   // std::addressof
#include <new>		   // placement new
#include <type_traits> // std::declval, std::enable_if_t, std
#include <utility>	   // std::forward

namespace snap
{

	namespace details
	{
		template <class U> SNAP_CONSTEXPR20 inline void* voidify(U& r) noexcept
		{
			return const_cast<void*>(static_cast<const void*>(std::addressof(r)));
		}
	} // namespace details

	template <class T,
			  class... Args,
			  std::enable_if_t<std::is_same_v<T, std::remove_cv_t<T>> && !std::is_array_v<T> && std::is_constructible_v<T, Args...>, int> = 0>
	SNAP_CONSTEXPR20 T* construct_at(T* location, Args&&... args)
	{
		assert(location && "null pointer given to construct_at(T*)");
		return ::new (details::voidify(*location)) T(std::forward<Args>(args)...);
	}

	template <class T,
			  class... Args,
			  std::enable_if_t<std::is_same_v<T, std::remove_cv_t<T>> && snap::is_bounded_array_v<T> && (sizeof...(Args) == 0), int> = 0>
	SNAP_CONSTEXPR20 T* construct_at(T* location, Args&&...)
	{
		assert(location && "null pointer given to construct_at(T* array)");
		::new (details::voidify(*location)) T[1]();
		return location; // pointer-to-array
	}

	template <class T, std::enable_if_t<!std::is_array_v<T> && std::is_destructible_v<T>, int> = 0> SNAP_CONSTEXPR20 void destroy_at(T* location)
	{
		assert(location && "null pointer given to destroy_at(T*)");
		location->~T();
	}

	template <class T, std::enable_if_t<std::is_array_v<T>, int> = 0> SNAP_CONSTEXPR20 void destroy_at(T* location)
	{
		assert(location && "null pointer given to destroy_at(T* array)");
		for (auto&& elem : *location) { ::snap::destroy_at(std::addressof(elem)); }
	}

} // namespace snap
