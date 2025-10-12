#pragma once

#include "snap/internal/builtin/bit/bit_cast.hpp"

#include <type_traits>

namespace snap
{
	namespace detail
	{
		template<class To, class From>
		using bit_cast_req = std::enable_if_t<
			(sizeof(To) == sizeof(From)) &&
			std::is_trivially_copyable_v<To> &&
			std::is_trivially_copyable_v<From>, int>;
	} // namespace detail

	template<class To, class From,
			 detail::bit_cast_req<To, From> = 0>
	constexpr To bit_cast(const From& from) noexcept
	{
		if constexpr (builtin::has_constexpr_bit_cast) {
			return builtin::bit_cast<To>(from);
		}
		else
		{
			using U = std::remove_const_t<To>;
			U u{};
			std::memcpy(&u, &from, sizeof(U));
			return static_cast<To>(u);
		}
	}
} // namespace snap
