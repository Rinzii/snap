#pragma once

#include "snap/internal/compat/constexpr.hpp"
#include "../internal/helpers/ptr_helpers.hpp"

namespace snap {
    template <class Smart, class Pointer, class... Args>
    class out_ptr_t {
        Smart& smart_;
        std::tuple<Args...> args_;
        Pointer ptr_; // e.g. int*, char*, etc.

        static constexpr bool is_shared_ptr_no_deleter =
            internal::is_std_shared_ptr<std::remove_cv_t<Smart>>::value &&
            sizeof...(Args) == 0;

        static_assert(!is_shared_ptr_no_deleter,
            "Resetting a std::shared_ptr without providing a deleter is forbidden by the standard.");

        static void reset_with_args(Smart& s, Pointer ptr, std::tuple<Args...>& args) {
            std::apply(
                [&](auto&&... unpacked) {
                    s.reset(ptr, std::forward<decltype(unpacked)>(unpacked)...);
                },
                args
            );
        }

    public:
        out_ptr_t(const out_ptr_t&) = delete;
        out_ptr_t& operator=(const out_ptr_t&) = delete;

        SNAP_CONSTEXPR20 out_ptr_t(Smart& s, Args&&... args) noexcept(std::conjunction_v<
            std::is_nothrow_constructible<std::tuple<Args...>, Args&&...>
        >)
            : smart_(s)
            , args_(std::forward<Args>(args)...)
            , ptr_(nullptr)
        {}

        SNAP_CONSTEXPR20 ~out_ptr_t() noexcept(noexcept(reset_with_args(smart_, ptr_, args_))) {
            reset_with_args(smart_, ptr_, args_);
        }

        operator Pointer*() noexcept { return &ptr_; }

        template <typename P = Pointer,
                  typename = std::enable_if_t<internal::void_ptr_convertible<P>::value>>
        operator void**() noexcept {
            return reinterpret_cast<void**>(&ptr_);
        }
    };

    template <class Smart, class Pointer, class... Args>
    SNAP_CONSTEXPR20 auto out_ptr(Smart& s, Args&&... args) {
        return out_ptr_t<Smart, Pointer, Args...>(
            s, std::forward<Args>(args)...
        );
    }

} // namespace snap
