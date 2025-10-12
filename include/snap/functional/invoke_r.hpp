#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace snap
{
	template <class R,
			  class F,
			  class... Args,
			  /** \cond DOXYGEN_EXCLUDE */
			  typename = std::enable_if_t<std::is_invocable_r_v<R, F, Args...>>
			  /** \endcond */>
	constexpr R invoke_r(F&& f, Args&&... args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>)
	{
		if constexpr (std::is_void_v<R>) { std::invoke(std::forward<F>(f), std::forward<Args>(args)...); }
		else { return static_cast<R>(std::invoke(std::forward<F>(f), std::forward<Args>(args)...)); }
	}
} // namespace snap
