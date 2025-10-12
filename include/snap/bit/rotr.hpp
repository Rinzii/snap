#pragma once

#include <cstdint> // For std::uint64_t
#include <limits>
#include <type_traits>

#if defined(_MSC_VER) && !defined(__clang__)
	#include "snap/type_traits/is_constant_evaluated.hpp"

	#include <cstdlib> // For _rotr and _rotr64 intrinsics
#endif

namespace snap
{
	template <class T, std::enable_if_t<std::is_unsigned_v<T>, bool> = true>
	constexpr T rotr(T t, int cnt) noexcept
	{
#if defined(_MSC_VER) && !defined(__clang__)
		// Allow for the use of compiler intrinsics if we are not being evaluated at compile time in msvc.
		if (!is_constant_evaluated())
		{
			// These func are not constexpr in msvc.
			if constexpr (std::is_same_v<T, unsigned int>) { return _rotr(t, cnt); }
			else if constexpr (std::is_same_v<T, std::uint64_t>) { return _rotr64(t, cnt); }
		}
#endif
		const unsigned int dig = std::numeric_limits<T>::digits;

		if ((static_cast<unsigned int>(cnt) % dig) == 0) { return t; }

		if (cnt < 0)
		{
			cnt *= -1;
			return (t << (static_cast<unsigned int>(cnt) % dig)) |
				   (t >> (dig - (static_cast<unsigned int>(cnt) % dig))); // rotr with negative cnt is similar to rotl
		}

		return (t >> (static_cast<unsigned int>(cnt) % dig)) | (t << (dig - (static_cast<unsigned int>(cnt) % dig)));
	}
} // namespace snap
