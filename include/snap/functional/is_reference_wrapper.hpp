#pragma once

#include <functional>
#include <type_traits>

namespace snap
{
    template<typename T>
    struct is_reference_wrapper : std::false_type {};

    template<typename U>
    struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
} // namespace snap
