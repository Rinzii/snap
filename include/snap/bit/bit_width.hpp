#pragma once

#include "snap/internal/helpers/bit_log2.hpp"
#include "snap/type_traits/is_integer.hpp"

#include <type_traits>

namespace snap
{
	template <typename T>
	constexpr auto bit_width(T t) noexcept -> std::enable_if_t<is_unsigned_integer_v<T>, int>
	{
		return t == 0 ? 0 : internal::bit_log2(t) + 1;
	}
} // namespace snap
