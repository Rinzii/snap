#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/compat/std.hpp"

#if SNAP_HAS_CPP20
	#include <concepts>
	#include <ranges>

SNAP_BEGIN_NAMESPACE
namespace concepts
{
	template <class R, class T>
	concept container_compatible_range = std::ranges::input_range<R> && std::constructible_from<T, std::ranges::range_reference_t<R>>;
} // namespace concepts
SNAP_END_NAMESPACE
#endif // SNAP_HAS_CPP20
