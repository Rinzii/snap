#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/is_brace_constructible.hpp"
#include "snap/internal/compat/constexpr.hpp"
#include "snap/internal/pp/no_unique_address.hpp"
#include "snap/type_traits/remove_cvref.hpp"

#include <initializer_list>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
template <class E>
	class unexpected;

	template <class T>
	struct is_unexpected : std::false_type
	{
	};

	template <class U>
	struct is_unexpected<unexpected<U>> : std::true_type
	{
	};

	template <class T>
	inline constexpr bool is_unexpected_v = is_unexpected<T>::value;

	template <class E>
	class unexpected
	{
		static_assert(std::is_object_v<E>, "unexpected<E>: E is an object type");
		static_assert(!std::is_array_v<E>, "unexpected<E>: E is not an array type");
		static_assert(!std::is_const_v<E> && !std::is_volatile_v<E>, "unexpected<E>: E is not cv-qualified");
		static_assert(!is_unexpected_v<E>, "unexpected<E>: E is not a specialization of unexpected");

	public:
		using error_type = E;

		constexpr unexpected(const unexpected&) = default;
		constexpr unexpected(unexpected&&) = default;
		constexpr unexpected& operator=(const unexpected&) = default;
		constexpr unexpected& operator=(unexpected&&) = default;

		SNAP_CONSTEXPR20 ~unexpected() = default;

		template <
			class Err,
			class = std::enable_if_t<
				!std::is_same_v<remove_cvref_t<Err>, unexpected> &&
				!std::is_same_v<remove_cvref_t<Err>, std::in_place_t> &&
				std::is_constructible_v<E, Err>>>
		explicit constexpr unexpected(Err&& e) noexcept(std::is_nothrow_constructible_v<E, Err&&>)
			: err_(std::forward<Err>(e))
		{
		}

		template <class... Args, class = std::enable_if_t<is_brace_constructible_v<E, Args&&...>>>
		explicit constexpr unexpected(std::in_place_t /*unused*/, Args&&... args) noexcept(noexcept(E{ std::forward<Args>(args)... }))
			: err_{ std::forward<Args>(args)... }
		{
		}

		template <class U, class... Args, class = std::enable_if_t<is_brace_constructible_v<E, std::initializer_list<U>&, Args&&...>>>
		explicit constexpr unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
			noexcept(noexcept(E{ il, std::forward<Args>(args)... }))
			: err_{ il, std::forward<Args>(args)... }
		{
		}

		constexpr const E& error() const& noexcept { return err_; }
		constexpr E& error() & noexcept { return err_; }
		constexpr const E&& error() const&& noexcept { return std::move(err_); }
		constexpr E&& error() && noexcept { return std::move(err_); }

		template <class X = E, class = std::enable_if_t<std::is_swappable_v<X>>>
		constexpr void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<X>)
		{
			using std::swap;
			swap(err_, other.err_);
		}

		template <class E2>
		friend constexpr bool operator==(const unexpected& x, const unexpected<E2>& y)
		{
			return x.error() == y.error();
		}

		template <class X = E, class = std::enable_if_t<std::is_swappable_v<X>>>
		friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y)))
		{
			x.swap(y);
		}

	private:
		SNAP_NO_UNIQUE_ADDRESS_ATTR E err_;
	};

	template <class E>
	unexpected(E) -> unexpected<E>;
SNAP_END_NAMESPACE
