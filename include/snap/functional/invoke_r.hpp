#pragma once

#include <type_traits>
#include <utility>
#include <functional>

namespace snap
{
    /**
     * @brief Invoke a callable and convert (or bind) its result to a specified return type `R`.
     *
     * @tparam R    Desired return type. May be `void`, a value type, or a reference type (e.g. `int&`).
     * @tparam F    Callable type; can be a function, function pointer, member pointer, lambda, functor,
     *              or `std::reference_wrapper`, invocable with `Args...`.
     * @tparam Args Argument types forwarded to the callable.
     * @param f     Callable to invoke.
     * @param args  Arguments forwarded to `f`.
     *
     * @return If `R` is `void`, returns nothing. Otherwise, returns the result of
     *         `std::invoke(f, args...)` explicitly converted to `R`.
     *
     * @par Constraints
     * Participates in overload resolution only if
     * `std::is_invocable_r<R, F, Args...>::value` is `true`.
     *
     * @par Exception Safety
     * `noexcept(std::is_nothrow_invocable_r<R, F, Args...>::value)`. The function is `noexcept`
     * if and only if invoking `f` with `args...` and converting the result to `R` is non-throwing.
     *
     * @remarks
     * - This mirrors the behavior of C++23 `std::invoke_r`, provided here for C++17.
     * - Uses `std::invoke` to support member pointers and other callables.
     * - Non-`void` results are converted with `static_cast<R>(...)` to match `invoke_r` semantics.
     *
     * @code
     * int add(int a, int b) { return a + b; }
     * int x = snap::invoke_r<int>(add, 2, 3);   // x == 5
     *
     * struct S { int m = 7; int mul(int v) const { return m * v; } };
     * S s;
     * int y = snap::invoke_r<int>(&S::mul, s, 3); // y == 21
     *
     * // void return:
     * void sink(int& acc, int v) { acc += v; }
     * int acc = 0;
     * snap::invoke_r<void>(sink, acc, 5);        // acc == 5
     * @endcode
     */
    template<class R, class F, class... Args,
             /** \cond DOXYGEN_EXCLUDE */
             typename = std::enable_if_t<std::is_invocable_r<R, F, Args...>::value>
             /** \endcond */>
    constexpr R invoke_r(F&& f, Args&&... args)
        noexcept(std::is_nothrow_invocable_r<R, F, Args...>::value)
    {
        if constexpr (std::is_void<R>::value) {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        } else {
            return static_cast<R>(
                std::invoke(std::forward<F>(f), std::forward<Args>(args)...)
            );
        }
    }
} // namespace snap
