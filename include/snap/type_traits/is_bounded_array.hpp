#pragma once

#include <type_traits>

namespace snap
{
	template<class T>
	struct is_bounded_array : std::false_type {};

	template<class T, std::size_t N>
	struct is_bounded_array<T[N]> : std::true_type {};

    template<class T>
    constexpr inline bool is_bounded_array_v = is_bounded_array<T>::value;
} // namespace snap
