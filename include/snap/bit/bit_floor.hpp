#pragma once

#include "snap/bit/bit_width.hpp"
#include "snap/type_traits/is_neither.hpp"

#include <limits>
#include <type_traits>

namespace snap {
	namespace detail {
		template <class T>
		inline constexpr bool is_not_invalid_for_bit_floor =
			std::is_unsigned_v<std::remove_cv_t<T>> &&
			is_neither_v<
				std::remove_cv_t<T>,
				bool,
				char,
	#if defined(__cpp_char8_t)
				char8_t,
	#endif
				char16_t,
				char32_t,
				wchar_t
			>;
	} // namespace detail

	template <class T, std::enable_if_t<detail::is_not_invalid_for_bit_floor<T>, bool> = true>
	constexpr T bit_floor(T x) noexcept {
		if (x != 0) {
			return static_cast<T>(T{1} << (bit_width(x) - 1));
		}
		return T{0};
	}
} // namespace snap
