#pragma once

#include "snap/type_traits/is_relation.hpp"

#include <type_traits>

namespace snap
{
	template <class R, class T, class U> struct is_equivalence_relation : is_relation<R, T, U>
	{
	};

	template <class R, class T, class U> inline constexpr bool is_equivalence_relation_v = is_equivalence_relation<R, T, U>::value;

} // namespace snap
