#pragma once

#include <type_traits>
#include <utility>
#include <initializer_list>

#include "snap/type_traits/remove_cvref.hpp"
#include "snap/type_traits/is_brace_constructible.hpp"

namespace snap {

// forward declaration for trait
template<class E> class unexpected;

// E must be: non-cv, object, not array, and not a specialization of snap::unexpected
template<class T> struct is_unexpected : std::false_type {};
template<class U> struct is_unexpected<unexpected<U>> : std::true_type {};

template<class T>
inline constexpr bool is_unexpected_v = is_unexpected<T>::value;

template<class E>
class unexpected {
    static_assert(std::is_object_v<E>, "unexpected<E>: E must be an object type");
    static_assert(!std::is_array_v<E>, "unexpected<E>: E must not be an array type");
    static_assert(!std::is_const_v<E> && !std::is_volatile_v<E>,
                  "unexpected<E>: E must not be cv-qualified");
    static_assert(!is_unexpected_v<E>,
                  "unexpected<E>: E must not be a specialization of unexpected");

public:
    using error_type = E;

    // 1,2) special members
    constexpr unexpected(const unexpected&) = default;
    constexpr unexpected(unexpected&&) = default;
    unexpected& operator=(const unexpected&) = default;
    unexpected& operator=(unexpected&&) = default;

    // 3) forwarding constructor (disabled for unexpected/in_place_t)
    template<class Err,
             class = std::enable_if_t<
                 !std::is_same_v<remove_cvref_t<Err>, unexpected> &&
                 !std::is_same_v<remove_cvref_t<Err>, std::in_place_t> &&
                 std::is_constructible_v<E, Err&&>
             >>
    explicit constexpr unexpected(Err&& e)
        noexcept(std::is_nothrow_constructible_v<E, Err&&>)
        : err_(std::forward<Err>(e)) {}

    // 4) in_place + args... (brace-init so aggregates work)
    template<class... Args,
             class = std::enable_if_t<is_brace_constructible_v<E, Args&&...>>>
    explicit constexpr unexpected(std::in_place_t, Args&&... args)
        noexcept(noexcept(E{std::forward<Args>(args)...}))
        : err_{std::forward<Args>(args)...} {}

    // 5) in_place + init_list + args... (also brace-init)
    template<class U, class... Args,
             class = std::enable_if_t<is_brace_constructible_v<E, std::initializer_list<U>&, Args&&...>>>
    explicit constexpr unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        noexcept(noexcept(E{il, std::forward<Args>(args)...}))
        : err_{il, std::forward<Args>(args)...} {}

    // error() accessors
    constexpr const E& error() const& noexcept { return err_; }
    constexpr E&       error() & noexcept      { return err_; }
    constexpr const E&& error() const&& noexcept { return std::move(err_); }
    constexpr E&&       error() && noexcept      { return std::move(err_); }

    // swap
    constexpr void swap(unexpected& other) noexcept(noexcept(std::swap(err_, other.err_))) {
        using std::swap;
        swap(err_, other.err_);
    }

    // friend operator== (ADL-only), enabled if comparable-to-bool
    template<class E2,
             class = std::enable_if_t<
                 std::is_convertible_v<decltype(std::declval<const E&>() == std::declval<const E2&>()), bool>
             >>
    friend constexpr bool operator==(const unexpected& x, const unexpected<E2>& y) {
        return x.error() == y.error();
    }

    // ADL swap, participates only if E is swappable
    template<class X = E,
             class = std::enable_if_t<std::is_swappable_v<X>>>
    friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

private:
    E err_;
};

// CTAD guide
template<class E>
unexpected(E) -> unexpected<E>;

} // namespace snap
