#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <atomic>
#include <memory>
#include <utility>

SNAP_BEGIN_NAMESPACE
struct nostopstate_t
        {
                explicit constexpr nostopstate_t(int) noexcept {}
        };

        inline constexpr nostopstate_t nostopstate{ 0 };

        class stop_source;

        class stop_token
        {
        public:
                stop_token() noexcept = default;
                stop_token(const stop_token&) noexcept = default;
                stop_token(stop_token&&) noexcept      = default;
                stop_token& operator=(const stop_token&) noexcept = default;
                stop_token& operator=(stop_token&&) noexcept = default;
                ~stop_token()                                 = default;

                void swap(stop_token& other) noexcept { state_.swap(other.state_); }

                [[nodiscard]] bool stop_requested() const noexcept
                {
                        return state_ && state_->load(std::memory_order_acquire);
                }

                [[nodiscard]] bool stop_possible() const noexcept { return static_cast<bool>(state_); }

                friend bool operator==(const stop_token& a, const stop_token& b) noexcept
                {
                        return a.state_ == b.state_;
                }

                friend void swap(stop_token& a, stop_token& b) noexcept { a.swap(b); }

        private:
                using state_ptr = std::shared_ptr<std::atomic<bool>>;

                explicit stop_token(state_ptr state) noexcept : state_(std::move(state)) {}

                state_ptr state_{};

                friend class stop_source;
        };

        class stop_source
        {
        public:
                stop_source() : state_(std::make_shared<std::atomic<bool>>(false)) {}
                explicit stop_source(nostopstate_t) noexcept {}

                stop_source(const stop_source&) noexcept = default;
                stop_source(stop_source&&) noexcept      = default;
                stop_source& operator=(const stop_source&) noexcept = default;
                stop_source& operator=(stop_source&&) noexcept = default;
                ~stop_source()                                     = default;

                void swap(stop_source& other) noexcept { state_.swap(other.state_); }

                [[nodiscard]] stop_token get_token() const noexcept
                {
                        if (!state_) { return stop_token{}; }
                        return stop_token(state_);
                }

                [[nodiscard]] bool stop_possible() const noexcept { return static_cast<bool>(state_); }

                bool request_stop() noexcept
                {
                        if (!state_) { return false; }
                        bool expected = false;
                        return state_->compare_exchange_strong(expected, true, std::memory_order_acq_rel);
                }

                friend void swap(stop_source& a, stop_source& b) noexcept { a.swap(b); }

        private:
                using state_ptr = std::shared_ptr<std::atomic<bool>>;
                state_ptr state_{};
        };

SNAP_END_NAMESPACE
