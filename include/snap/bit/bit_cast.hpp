#pragma once

#include <type_traits>


#include "snap/internal/builtin/bit_cast.hpp" // provides snap::builtin::has_constexpr_bit_cast and the macro

#ifdef SNAP_HAS_BUILTIN_BIT_CAST
	#define SNAP_BITCAST_CONSTEXPR constexpr
#else
	#include "snap/internal/compat/constexpr.hpp"
	#include <cstring> // memcpy fallback
	#define SNAP_BITCAST_CONSTEXPR SNAP_CONSTEXPR20
#endif

#ifdef SNAP_HAS_BUILTIN_BIT_CAST
  #undef SNAP_HAS_BUILTIN_BIT_CAST
#endif

namespace snap {

	template <typename To, typename From>
	SNAP_BITCAST_CONSTEXPR
	std::enable_if_t<
		sizeof(To) == sizeof(From) &&
		std::is_trivially_copyable_v<To> &&
		std::is_trivially_copyable_v<From>,
		To>
	bit_cast(const From& from) noexcept
	{
		if constexpr (builtin::has_constexpr_bit_cast) {
			return builtin::bit_cast<To>(from);
		} else
		{
			using U = std::remove_const_t<To>;
			alignas(U) unsigned char storage[sizeof(U)];
			std::memcpy(storage, &from, sizeof(U));
			U* p = std::launder(reinterpret_cast<U*>(storage));
			return static_cast<To>(*p);
		}
	}

} // namespace snap
