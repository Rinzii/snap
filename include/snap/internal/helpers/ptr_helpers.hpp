#pragma once

#include <tuple>
#include <memory>
#include <type_traits>
#include <utility>

namespace snap::internal {

        template <typename Pointer>
        using void_ptr_convertible =
            std::is_convertible<Pointer*, void**>;

        template <typename Smart>
        struct is_std_shared_ptr : std::false_type {};

        template <typename T>
        struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

} // namespace snap::internal
