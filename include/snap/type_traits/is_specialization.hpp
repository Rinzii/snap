#pragma once

/**
 * @file
 * @brief Detection trait for checking whether a type is a specialization of a
 *        given class template.
 *
 * @details
 * This header provides a single variable template:
 * `snap::is_specialization_v<T, Template>`. It evaluates to `true` if and only
 * if `T` is an instantiation of the class template `Template<...>`, and
 * `false` otherwise.
 *
 * This is a lightweight, implementation-level utility intended to fill a common
 * need until the language/library offers an official facility. It mirrors the
 * shape of proposals that explored this problem but keeps the scope minimal.
 *
 * @note
 * Qualifiers matter. If `T` has cv/ref qualifications (e.g. `const`, `&`,
 * `&&`), it will not be considered a match. If you want to ignore
 * cv/ref-qualifiers, strip them first:
 *
 * @code
 * #include <type_traits>
 *
 * template <class T, template<class...> class Template>
 * inline constexpr bool is_specialization_cvref_insensitive_v =
 *     snap::is_specialization_v<std::remove_cvref_t<T>, Template>;
 * @endcode
 *
 * @par Example
 * @code
 * template <class...> struct foo;
 * struct bar {};
 *
 * static_assert( snap::is_specialization_v<foo<int, double>, foo> ); // true
 * static_assert(!snap::is_specialization_v<bar, foo> );              // false
 * static_assert(!snap::is_specialization_v<const foo<int>, foo> );   // false (cv-qualified)
 * @endcode
 */

namespace snap
{
    /**
     * @brief Primary template: `false` for all types by default.
     *
     * @tparam T        The type to test.
     * @tparam Template The class template to test against (e.g. `std::tuple`).
     *
     * @remarks
     * This primary template intentionally yields `false`. A partial
     * specialization below flips the value to `true` when `T` is of the form
     * `Template<Args...>`.
     */
    template <class T, template <class...> class Template>
    inline constexpr bool is_specialization_v = false;

    /**
     * @brief Partial specialization: `true` when `T` matches `Template<Args...>`.
     *
     * @tparam Template The class template being matched.
     * @tparam Args     The deduced template arguments of `T`.
     */
    template <template <class...> class Template, class... Args>
    inline constexpr bool is_specialization_v<Template<Args...>, Template> = true;

} // namespace snap
