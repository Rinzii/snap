#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <utility> // std::swap

SNAP_BEGIN_NAMESPACE
class stop_source;						// forward decl (friend)
	template <class T> class stop_callback; // forward decl (friend)

	// C++17 stop_token backed by snap::stop_state via intrusive refcounting.
	class stop_token
	{
	public:
		stop_token() noexcept = default;

		stop_token(const stop_token&) noexcept			  = default;
		stop_token(stop_token&&) noexcept				  = default;
		stop_token& operator=(const stop_token&) noexcept = default;
		stop_token& operator=(stop_token&&) noexcept	  = default;
		~stop_token()									  = default;

		void swap(stop_token& other) noexcept { state_.swap(other.state_); }

		[[nodiscard]] bool stop_requested() const noexcept { return static_cast<bool>(state_) && state_->stop_requested(); }

		[[nodiscard]] bool stop_possible() const noexcept { return static_cast<bool>(state_) && state_->stop_possible_for_token(); }

		friend bool operator==(const stop_token& a, const stop_token& b) noexcept { return a.state_ == b.state_; }

		friend void swap(stop_token& a, stop_token& b) noexcept { a.swap(b); }

	private:
		intrusive_shared_ptr<stop_state> state_{};

		// Constructible by associated types
		explicit stop_token(const intrusive_shared_ptr<stop_state>& s) : state_(s) {}

		friend class stop_source;
		template <class T> friend class stop_callback;
	};

SNAP_END_NAMESPACE
