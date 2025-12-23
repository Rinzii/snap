#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/stop_token/intrusive_shared_ptr.hpp"
#include "snap/stop_token/stop_state.hpp"

#include <utility>

SNAP_BEGIN_NAMESPACE

struct nostopstate_t
{
	explicit constexpr nostopstate_t() noexcept = default;
};

inline constexpr nostopstate_t nostopstate{};

class stop_source;

class stop_token
{
public:
	stop_token() noexcept							  = default;
	stop_token(const stop_token&) noexcept			  = default;
	stop_token(stop_token&&) noexcept				  = default;
	stop_token& operator=(const stop_token&) noexcept = default;
	stop_token& operator=(stop_token&&) noexcept	  = default;
	~stop_token()									  = default;

	void swap(stop_token& other) noexcept { state_.swap(other.state_); }

	[[nodiscard]] bool stop_requested() const noexcept { return state_ && state_->stop_requested(); }

	[[nodiscard]] bool stop_possible() const noexcept { return state_ && state_->stop_possible_for_token(); }

	friend bool operator==(const stop_token& a, const stop_token& b) noexcept { return a.state_ == b.state_; }
	friend bool operator!=(const stop_token& a, const stop_token& b) noexcept { return !(a == b); }

	friend void swap(stop_token& a, stop_token& b) noexcept { a.swap(b); }

private:
	intrusive_shared_ptr<stop_state> state_{};

	explicit stop_token(intrusive_shared_ptr<stop_state> state) noexcept : state_(std::move(state)) {}

	friend class stop_source;
};

class stop_source
{
public:
	stop_source() : state_(new stop_state()) { state_->increment_stop_source_counter(); }

	explicit stop_source(nostopstate_t) noexcept {}

	stop_source(const stop_source& other) noexcept : state_(other.state_)
	{
		if (state_) { state_->increment_stop_source_counter(); }
	}

	stop_source(stop_source&& other) noexcept = default;

	stop_source& operator=(const stop_source& other) noexcept
	{
		if (other.state_) { other.state_->increment_stop_source_counter(); }
		if (state_) { state_->decrement_stop_source_counter(); }
		state_ = other.state_;
		return *this;
	}

	stop_source& operator=(stop_source&& other) noexcept = default;

	~stop_source()
	{
		if (state_) { state_->decrement_stop_source_counter(); }
	}

	void swap(stop_source& other) noexcept { state_.swap(other.state_); }

	[[nodiscard]] stop_token get_token() const noexcept { return stop_token(state_); }

	[[nodiscard]] bool stop_possible() const noexcept { return static_cast<bool>(state_); }

	[[nodiscard]] bool stop_requested() const noexcept { return state_ && state_->stop_requested(); }

	bool request_stop() noexcept { return state_ && state_->request_stop(); }

	friend bool operator==(const stop_source& a, const stop_source& b) noexcept { return a.state_ == b.state_; }
	friend bool operator!=(const stop_source& a, const stop_source& b) noexcept { return !(a == b); }

	friend void swap(stop_source& a, stop_source& b) noexcept { a.swap(b); }

private:
	intrusive_shared_ptr<stop_state> state_{};
};

SNAP_END_NAMESPACE
