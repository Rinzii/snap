#pragma once

namespace snap
{
	template <auto V> struct nontype_t
	{
		explicit nontype_t() = default;
	};
	template <auto V> inline constexpr nontype_t<V> nontype{};
} // namespace snap
