#pragma once

#include <type_traits>

namespace riot::internal
{
	/**
	 * @brief Helper function to prevent the user from accidentally passing an integer to an expect function.
	 */
	template <typename T, std::enable_if_t<std::is_same_v<T, bool>, bool> = true>
	constexpr auto expects_bool_condition(T value, [[maybe_unused]] bool expected) -> bool
	{
		#if defined(__GNUC__) || defined(__clang__)
		return __builtin_expect(value, expected);
		#else
		return value;
		#endif // defined(__GNUC__) || defined(__clang__)
	}
} // namespace riot::internal