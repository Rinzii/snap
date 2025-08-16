#pragma once

#include <type_traits>
#include <utility>

#include "snap/meta/detector.hpp"

namespace snap {

    // Detection helpers
    template<class T> using pointer_member_t              = typename T::pointer;
    template<class T> using element_type_member_t         = typename T::element_type;
    template<class T> using pointer_traits_element_type_t = typename std::pointer_traits<T>::element_type;

    // pointer_of<T>: defines 'type' in the first matching case:
    //  1) T::pointer
    //  2) T::element_type*
    //  3) std::pointer_traits<T>::element_type*
    // If none match, 'type' is absent (use pointer_of_or_t for a fallback).
    template<class T, class Enable = void>
    struct pointer_of { /* no 'type' by default */ };

    // 1) Prefer 'pointer' member if present
    template<class T>
    struct pointer_of<T, std::enable_if_t<is_detected_v<pointer_member_t, T>, void>> {
        using type = typename T::pointer;
    };

    // 2) Else use 'element_type*' if present (and no 'pointer')
    template<class T>
    struct pointer_of<T, std::enable_if_t<
        !is_detected_v<pointer_member_t, T> && is_detected_v<element_type_member_t, T>, void>> {
        using type = typename T::element_type*;
    };

    // 3) Else use 'pointer_traits<T>::element_type*' if available
    template<class T>
    struct pointer_of<T, std::enable_if_t<
        !is_detected_v<pointer_member_t, T> &&
        !is_detected_v<element_type_member_t, T> &&
         is_detected_v<pointer_traits_element_type_t, T>, void>> {
        using type = typename std::pointer_traits<T>::element_type*;
    };

    // Alias to extract the detected type (SFINAE-friendly)
    template<class T>
    using pointer_of_t = typename pointer_of<T>::type;

    // Fallback: if pointer_of_t<T> is not detectable, use U instead
    template<class T, class U>
    using pointer_of_or_t = detected_or_t<U, pointer_of_t, T>;
} // namespace snap
