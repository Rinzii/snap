#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/helpers/perfect_forward.hpp"

#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
namespace detail
	{

		struct bind_front_op
		{
			template <class... Args> constexpr auto operator()(Args&&... args) const noexcept(noexcept(std::invoke(std::forward<Args>(args)...)))
				-> decltype(std::invoke(std::forward<Args>(args)...))
			{
				return std::invoke(std::forward<Args>(args)...);
			}
		};

		template <class Fn, class... BoundArgs> struct bind_front_t : internal::perfect_forward<bind_front_op, Fn, BoundArgs...>
		{
			using internal::perfect_forward<bind_front_op, Fn, BoundArgs...>::perfect_forward;
		};

	} // namespace detail

	template <class Fn,
			  class... Args,
			  std::enable_if_t<std::is_constructible_v<std::decay_t<Fn>, Fn> && std::is_move_constructible_v<std::decay_t<Fn>> &&
								   (std::is_constructible_v<std::decay_t<Args>, Args> && ...) && (std::is_move_constructible_v<std::decay_t<Args>> && ...),
							   int> = 0>
	constexpr auto bind_front(Fn&& f, Args&&... args) -> detail::bind_front_t<std::decay_t<Fn>, std::decay_t<Args>...>
	{
		return detail::bind_front_t<std::decay_t<Fn>, std::decay_t<Args>...>(std::forward<Fn>(f), std::forward<Args>(args)...);
	}

SNAP_END_NAMESPACE
