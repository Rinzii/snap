#ifndef SNP_INCLUDE_SNAP_UTILITY_FUNCTION_REF_HPP
#define SNP_INCLUDE_SNAP_UTILITY_FUNCTION_REF_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
namespace details
{
	template <class Sig> struct signature_qualifiers;

	template <class R, class... Args> struct signature_qualifiers<R(Args...)>
	{
		using function					  = R(Args...);
		static constexpr bool is_noexcept = false;

		template <class... T> static constexpr bool is_invocable_using = std::is_invocable_r_v<R, T..., Args...>;

		template <class T> using cv = T;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) noexcept>
	{
		using function					  = R(Args...);
		static constexpr bool is_noexcept = true;

		template <class... T> static constexpr bool is_invocable_using = std::is_nothrow_invocable_r_v<R, T..., Args...>;

		template <class T> using cv = T;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) const> : signature_qualifiers<R(Args...)>
	{
		template <class T> using cv = T const;
	};

	template <class R, class... Args> struct signature_qualifiers<R(Args...) const noexcept> : signature_qualifiers<R(Args...) noexcept>
	{
		template <class T> using cv = T const;
	};

	template <bool Noexcept, class R, class... Args> struct function_ref_base
	{
		using invoker_t = R (*)(void*, Args...) noexcept(Noexcept);

		void* object	  = nullptr;
		invoker_t invoker = nullptr;

		template <class F> void bind(F* ptr) noexcept
		{
		object	= const_cast<void*>(static_cast<const void*>(ptr)); // NOLINT(cppcoreguidelines-pro-type-const-cast)
			invoker = [](void* obj, Args... call_args) noexcept(Noexcept) -> R { return std::invoke(*static_cast<F*>(obj), std::forward<Args>(call_args)...); };
		}
	};
} // namespace details

template <class Signature> class function_ref;

template <class R, class... Args> class function_ref<R(Args...)> : private details::function_ref_base<false, R, Args...>
{
	using base	  = details::function_ref_base<false, R, Args...>;
	using decay_t = std::decay_t<R>;

public:
	function_ref()				 = delete;
	function_ref(std::nullptr_t) = delete;
	~function_ref()				 = default;

	template <class F,
			  std::enable_if_t<!std::is_same_v<std::remove_reference_t<F>, function_ref> && std::is_invocable_r_v<R, F&, Args...> &&
								   (std::is_lvalue_reference_v<F> || std::is_pointer_v<std::decay_t<F>> || std::is_function_v<std::remove_reference_t<F>>),
							   int> = 0>
	explicit function_ref(F&& f) noexcept
	{
		bind_callable(std::forward<F>(f));
	}

	function_ref(const function_ref&)			 = default;
	function_ref& operator=(const function_ref&) = default;
	function_ref(function_ref&&)				 = default;
	function_ref& operator=(function_ref&&)		 = default;

	R operator()(Args... args) const
	{
		return fn_ptr_ ? std::invoke(fn_ptr_, std::forward<Args>(args)...) : base::invoker(base::object, std::forward<Args>(args)...);
	}

	explicit operator bool() const noexcept { return fn_ptr_ != nullptr || base::invoker != nullptr; }

private:
	R (*fn_ptr_)(Args...) = nullptr;

	template <class F> void bind_callable(F&& f) noexcept
	{
		auto&& callable = std::forward<F>(f);
		if constexpr (std::is_function_v<std::remove_reference_t<F>>) { fn_ptr_ = std::addressof(callable); }
		else if constexpr (std::is_pointer_v<std::decay_t<F>> && std::is_function_v<std::remove_pointer_t<std::decay_t<F>>>) { fn_ptr_ = callable; }
		else
		{
			using stored_t = std::remove_reference_t<F>;
			base::template bind<stored_t>(std::addressof(callable));
		}
	}
};

template <class R, class... Args> class function_ref<R(Args...) noexcept> : private details::function_ref_base<true, R, Args...>
{
	using base = details::function_ref_base<true, R, Args...>;

public:
	function_ref()				 = delete;
	function_ref(std::nullptr_t) = delete;
	~function_ref()				 = default;

	template <class F,
			  std::enable_if_t<!std::is_same_v<std::remove_reference_t<F>, function_ref> && std::is_nothrow_invocable_r_v<R, F&, Args...> &&
								   (std::is_lvalue_reference_v<F> || std::is_pointer_v<std::decay_t<F>> || std::is_function_v<std::remove_reference_t<F>>),
							   int> = 0>
	explicit function_ref(F&& f) noexcept
	{
		bind_callable(std::forward<F>(f));
	}

	function_ref(const function_ref&)			 = default;
	function_ref& operator=(const function_ref&) = default;
	function_ref(function_ref&&)				 = default;
	function_ref& operator=(function_ref&&)		 = default;

	R operator()(Args... args) const noexcept
	{
		return fn_ptr_ ? std::invoke(fn_ptr_, std::forward<Args>(args)...) : base::invoker(base::object, std::forward<Args>(args)...);
	}

	explicit operator bool() const noexcept { return fn_ptr_ != nullptr || base::invoker != nullptr; }

private:
	R (*fn_ptr_)(Args...) noexcept = nullptr;

	template <class F> void bind_callable(F&& f) noexcept
	{
		auto&& callable = std::forward<F>(f);
		if constexpr (std::is_function_v<std::remove_reference_t<F>>) { fn_ptr_ = std::addressof(callable); }
		else if constexpr (std::is_pointer_v<std::decay_t<F>> && std::is_function_v<std::remove_pointer_t<std::decay_t<F>>>) { fn_ptr_ = callable; }
		else
		{
			using stored_t = std::remove_reference_t<F>;
			base::template bind<stored_t>(std::addressof(callable));
		}
	}
};

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_UTILITY_FUNCTION_REF_HPP
