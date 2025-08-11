

#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace snap::internal {

    // Forward declaration
    template <class Op, class Indices, class... BoundArgs>
    struct perfect_forward_impl;

    // Implementation over an index sequence
    template <class Op, std::size_t... I, class... BoundArgs>
    struct perfect_forward_impl<Op, std::index_sequence<I...>, BoundArgs...> {
    private:
        std::tuple<BoundArgs...> bound_args_;

    public:
        // Construct from anything that can build the bound tuple
        template <class... Args,
            class = std::enable_if_t<
                std::is_constructible_v<std::tuple<BoundArgs...>, Args&&...>>>
        explicit constexpr perfect_forward_impl(Args&&... bound_args)
            noexcept(std::is_nothrow_constructible_v<std::tuple<BoundArgs...>, Args&&...>)
            : bound_args_(std::forward<Args>(bound_args)...) {}

        perfect_forward_impl(const perfect_forward_impl&) = default;
        perfect_forward_impl(perfect_forward_impl&&)      = default;
        perfect_forward_impl& operator=(const perfect_forward_impl&) = default;
        perfect_forward_impl& operator=(perfect_forward_impl&&)      = default;

        // & overload
        template <class... Args,
            class = std::enable_if_t<std::is_invocable_v<Op, BoundArgs&..., Args...>>>
        constexpr auto operator()(Args&&... args) &
            noexcept(noexcept(Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...)))
            -> decltype(Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...))
        {
            return Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...);
        }
        template <class... Args,
            class = std::enable_if_t<!std::is_invocable_v<Op, BoundArgs&..., Args...>>>
        auto operator()(Args&&...) & = delete;

        // const& overload
        template <class... Args,
            class = std::enable_if_t<std::is_invocable_v<Op, const BoundArgs&..., Args...>>>
        constexpr auto operator()(Args&&... args) const&
            noexcept(noexcept(Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...)))
            -> decltype(Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...))
        {
            return Op{}(std::get<I>(bound_args_)..., std::forward<Args>(args)...);
        }
        template <class... Args,
            class = std::enable_if_t<!std::is_invocable_v<Op, const BoundArgs&..., Args...>>>
        auto operator()(Args&&...) const& = delete;

        // && overload
        template <class... Args,
            class = std::enable_if_t<std::is_invocable_v<Op, BoundArgs..., Args...>>>
        constexpr auto operator()(Args&&... args) &&
            noexcept(noexcept(Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...)))
            -> decltype(Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...))
        {
            return Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...);
        }
        template <class... Args,
            class = std::enable_if_t<!std::is_invocable_v<Op, BoundArgs..., Args...>>>
        auto operator()(Args&&...) && = delete;

        // const&& overload
        template <class... Args,
            class = std::enable_if_t<std::is_invocable_v<Op, const BoundArgs..., Args...>>>
        constexpr auto operator()(Args&&... args) const&&
            noexcept(noexcept(Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...)))
            -> decltype(Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...))
        {
            return Op{}(std::get<I>(std::move(bound_args_))..., std::forward<Args>(args)...);
        }
        template <class... Args,
            class = std::enable_if_t<!std::is_invocable_v<Op, const BoundArgs..., Args...>>>
        auto operator()(Args&&...) const&& = delete;
    };

    // Alias that builds the index sequence for BoundArgs...
    template <class Op, class... Args>
    using perfect_forward = perfect_forward_impl<Op, std::index_sequence_for<Args...>, Args...>;

}
