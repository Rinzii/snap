#pragma once

#include "snap/internal/builtin/type_traits/is_constant_evaluated.hpp"

namespace snap
{
	constexpr bool is_constant_evaluated() noexcept
	{
		return snap::builtin::is_constant_evaluated();
	}
} // namespace snap
