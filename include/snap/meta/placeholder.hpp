#pragma once

#include "snap/meta/always_false.hpp"

namespace snap
{
	struct placeholder_t {
		placeholder_t() = default;

		template<class... Ts>
		constexpr placeholder_t(Ts&&...) noexcept {}

		template<class T>
		[[nodiscard]] operator T() const noexcept {
			static_assert(snap::always_false<T>, "conversion of snap::placeholder_t is ill-formed");
		}
	};
}
