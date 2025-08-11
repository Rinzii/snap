#pragma once
#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>

#include "snap/internal/helpers/perfect_forward.hpp"

namespace snap {
namespace detail {

// Builds an index pack [0, NBound) used to expand the bound tuple at call sites.
template <std::size_t NBound, class Indices = std::make_index_sequence<NBound>>
struct bind_back_op;

template <std::size_t NBound, std::size_t... Ip>
struct bind_back_op<NBound, std::index_sequence<Ip...>> {
    template <class Fn, class BoundTuple, class... Args>
    constexpr auto operator()(Fn&& f, BoundTuple&& bound, Args&&... args) const
        // noexcept/return type mirror the underlying std::invoke
        noexcept(noexcept(std::invoke(std::forward<Fn>(f),
                                      std::forward<Args>(args)...,
                                      std::get<Ip>(std::forward<BoundTuple>(bound))...)))
        -> decltype(std::invoke(std::forward<Fn>(f),
                                std::forward<Args>(args)...,
                                std::get<Ip>(std::forward<BoundTuple>(bound))...))
    {
        // Call f with unbound args first, then expand bound tuple elements at the end.
        return std::invoke(std::forward<Fn>(f),
                           std::forward<Args>(args)...,
                           std::get<Ip>(std::forward<BoundTuple>(bound))...);
    }
};

// Reuse the generic perfect-forwarding wrapper; it stores (Fn, BoundTuple)
// and provides the 4 ref-qualified operator() overloads.
template <class Fn, class BoundTuple>
struct bind_back_t
    : internal::perfect_forward< bind_back_op< std::tuple_size_v<BoundTuple> >,
                                 Fn, BoundTuple >
{
    using internal::perfect_forward< bind_back_op< std::tuple_size_v<BoundTuple> >,
                                     Fn, BoundTuple >::perfect_forward;
};

} // namespace detail

// Factory: decay stored types and package bound args into a tuple.
// SFINAE enforces constructible + move-constructible mandates.
// Overall noexcept depends on constructing the wrapper.
template <class Fn, class... Args,
          class = std::enable_if_t<
              std::is_constructible_v<std::decay_t<Fn>, Fn> &&
              std::is_move_constructible_v<std::decay_t<Fn>> &&
              (std::is_constructible_v<std::decay_t<Args>, Args> && ...) &&
              (std::is_move_constructible_v<std::decay_t<Args>> && ...)>>
constexpr auto bind_back(Fn&& f, Args&&... args)
    noexcept(noexcept(detail::bind_back_t<std::decay_t<Fn>, std::tuple<std::decay_t<Args>...>>(
        std::forward<Fn>(f),
        std::forward_as_tuple(std::forward<Args>(args)...))))
    -> detail::bind_back_t<std::decay_t<Fn>, std::tuple<std::decay_t<Args>...>>
{
    return detail::bind_back_t<std::decay_t<Fn>, std::tuple<std::decay_t<Args>...>>(
        std::forward<Fn>(f),
        std::forward_as_tuple(std::forward<Args>(args)...));
}

} // namespace snap
