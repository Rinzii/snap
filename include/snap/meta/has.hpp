#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/detector.hpp"
#include "snap/type_traits/remove_cvref.hpp"

#include <cstddef>
#include <functional>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE

/**
 * \brief Expression probing utilities built on the detection idiom.
 */
namespace probe
{
	/**
	 * \brief True if `Expr<Ts...>` is a valid type expression.
	 */
	template <template <class...> class Expr, class... Ts> inline constexpr bool has = ::SNAP_NAMESPACE::is_detected_v<Expr, Ts...>;

	/**
	 * \brief True if `Expr<Ts...>` is valid and convertible to `To`.
	 */
	template <template <class...> class Expr, class To, class... Ts> inline constexpr bool has_as =
		::SNAP_NAMESPACE::is_detected_convertible_v<To, Expr, Ts...>;

	/**
	 * \brief True if `Expr<T, U>` or `Expr<U, T>` is valid.
	 */
	template <template <class...> class Expr, class T, class U> inline constexpr bool has_either = has<Expr, T, U> || has<Expr, U, T>;

	/**
	 * \brief True if `Expr<T, U>` or `Expr<U, T>` is valid and convertible to `To`.
	 */
	template <template <class...> class Expr, class To, class T, class U> inline constexpr bool has_either_as =
		has_as<Expr, To, T, U> || has_as<Expr, To, U, T>;

	/**
	 * \brief Conjunction of boolean template arguments.
	 */
	template <bool... Bs> inline constexpr bool all = (Bs && ...);

	/**
	 * \brief Disjunction of boolean template arguments.
	 */
	template <bool... Bs> inline constexpr bool any = (Bs || ...);

	/**
	 * \brief Type-level list of expression templates.
	 */
	template <template <class...> class...> struct expr_list
	{
	};

	/**
	 * \brief Alias for an expression list intended as an "all of" set.
	 */
	template <template <class...> class... Exprs> using all_of = expr_list<Exprs...>;

	/**
	 * \brief Alias for an expression list intended as an "any of" set.
	 */
	template <template <class...> class... Exprs> using any_of = expr_list<Exprs...>;

	/**
	 * \brief Primary template for `has_all`.
	 */
	template <class ExprList, class... Ts> inline constexpr bool has_all = false;

	/**
	 * \brief True if every expression in the list is valid for `Ts...`.
	 */
	template <template <class...> class... Exprs, class... Ts> inline constexpr bool has_all<expr_list<Exprs...>, Ts...> = all<has<Exprs, Ts...>...>;

	/**
	 * \brief Primary template for `has_any`.
	 */
	template <class ExprList, class... Ts> inline constexpr bool has_any = false;

	/**
	 * \brief True if at least one expression in the list is valid for `Ts...`.
	 */
	template <template <class...> class... Exprs, class... Ts> inline constexpr bool has_any<expr_list<Exprs...>, Ts...> = any<has<Exprs, Ts...>...>;

	/**
	 * \brief Shorthand alias for an expression list.
	 */
	template <template <class...> class... Exprs> using exprs = expr_list<Exprs...>;

	/**
	 * \brief Cv/ref form helpers.
	 */
	namespace ref
	{
		/**
		 * \brief Value form: `T`.
		 */
		template <class T> using val = T;

		/**
		 * \brief Const value form: `T const`.
		 */
		template <class T> using cval = T const;

		/**
		 * \brief Lvalue reference form: `T&`.
		 */
		template <class T> using lref = T&;

		/**
		 * \brief Rvalue reference form: `T&&`.
		 */
		template <class T> using rref = T&&;

		/**
		 * \brief Const lvalue reference form: `T const&`.
		 */
		template <class T> using cref = T const&;

		/**
		 * \brief Const rvalue reference form: `T const&&`.
		 */
		template <class T> using crref = T const&&;
	} // namespace ref

	/**
	 * \brief Compatibility alias for cv/ref helpers.
	 */
	namespace vc = ref; // namespace vc

	/**
	 * \brief Argument form markers for multi-form probing.
	 */
	namespace form
	{
		/**
		 * \brief Marker that expands a type into multiple cv/ref forms when probing.
		 */
		template <class T> struct any_cvref
		{
			using type = T;
		};

		/**
		 * \brief Marker that keeps a type fixed when probing.
		 */
		template <class T> struct fixed
		{
			using type = T;
		};
	} // namespace form

	/**
	 * \brief Compatibility alias for argument form markers.
	 */
	namespace qual = form; // namespace qual

	/**
	 * \brief Implementation details for form expansion.
	 */
	namespace detail
	{
		/**
		 * \brief Simple type list.
		 */
		template <class...> struct type_list
		{
		};

		/**
		 * \brief Primary template for form expansion and probing.
		 */
		template <template <class...> class Expr, class Remaining, class... Built> struct detect_any_form_impl;

		/**
		 * \brief Base case for form expansion.
		 */
		template <template <class...> class Expr, class... Built> struct detect_any_form_impl<Expr, type_list<>, Built...>
		{
			static constexpr bool value = ::SNAP_NAMESPACE::is_detected_v<Expr, Built...>;
		};

		/**
		 * \brief Fixed form case.
		 */
		template <template <class...> class Expr, class T, class... Rest, class... Built>
		struct detect_any_form_impl<Expr, type_list<form::fixed<T>, Rest...>, Built...>
		{
			static constexpr bool value = detect_any_form_impl<Expr, type_list<Rest...>, Built..., T>::value;
		};

		/**
		 * \brief any-cvref form case.
		 */
		template <template <class...> class Expr, class T, class... Rest, class... Built>
		struct detect_any_form_impl<Expr, type_list<form::any_cvref<T>, Rest...>, Built...>
		{
			static constexpr bool value = detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::val<T>>::value ||
										  detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::cval<T>>::value ||
										  detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::lref<T>>::value ||
										  detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::cref<T>>::value ||
										  detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::rref<T>>::value ||
										  detect_any_form_impl<Expr, type_list<Rest...>, Built..., ref::crref<T>>::value;
		};
	} // namespace detail

	/**
	 * \brief True if `Expr` is valid for at least one form-combination implied by `Forms...`.
	 */
	template <template <class...> class Expr, class... Forms> inline constexpr bool detect_any_form_v =
		detail::detect_any_form_impl<Expr, detail::type_list<Forms...>>::value;

	/**
	 * \brief True if `Expr<Ts...>` is valid for at least one cv/ref expansion of each `Ts`.
	 */
	template <template <class...> class Expr, class... Ts> inline constexpr bool detect_any_cvref_v = detect_any_form_v<Expr, form::any_cvref<Ts>...>;

	/**
	 * \brief Primary template for `all_exprs_detect_any_form_v`.
	 */
	template <class ExprList, class... Forms> inline constexpr bool all_exprs_detect_any_form_v = false;

	/**
	 * \brief True if every expression in the list is valid for at least one form-combination.
	 */
	template <template <class...> class... Exprs, class... Forms> inline constexpr bool all_exprs_detect_any_form_v<expr_list<Exprs...>, Forms...> =
		all<detect_any_form_v<Exprs, Forms...>...>;

	/**
	 * \brief Primary template for `any_expr_detect_any_form_v`.
	 */
	template <class ExprList, class... Forms> inline constexpr bool any_expr_detect_any_form_v = false;

	/**
	 * \brief True if at least one expression in the list is valid for at least one form-combination.
	 */
	template <template <class...> class... Exprs, class... Forms> inline constexpr bool any_expr_detect_any_form_v<expr_list<Exprs...>, Forms...> =
		any<detect_any_form_v<Exprs, Forms...>...>;

	/**
	 * \brief True if every expression in the list is valid for at least one cv/ref expansion of `Ts...`.
	 */
	template <class ExprList, class... Ts> inline constexpr bool all_exprs_detect_any_cvref_v = all_exprs_detect_any_form_v<ExprList, form::any_cvref<Ts>...>;

	/**
	 * \brief True if at least one expression in the list is valid for at least one cv/ref expansion of `Ts...`.
	 */
	template <class ExprList, class... Ts> inline constexpr bool any_expr_detect_any_cvref_v = any_expr_detect_any_form_v<ExprList, form::any_cvref<Ts>...>;

	/**
	 * \brief Compatibility alias for older naming.
	 */
	template <template <class...> class Expr, class... Qualified> inline constexpr bool detects_any_qualifiers_v = detect_any_form_v<Expr, Qualified...>;

	/**
	 * \brief Compatibility alias for older naming.
	 */
	template <template <class...> class Expr, class... Ts> inline constexpr bool detects_any_cvref_v = detect_any_cvref_v<Expr, Ts...>;

	/**
	 * \brief Compatibility alias for older naming.
	 */
	template <class ExprList, class... Qualified> inline constexpr bool all_exprs_detect_any_qualifiers_v = all_exprs_detect_any_form_v<ExprList, Qualified...>;

	/**
	 * \brief Compatibility alias for older naming.
	 */
	template <class ExprList, class... Qualified> inline constexpr bool any_expr_detect_any_qualifiers_v = any_expr_detect_any_form_v<ExprList, Qualified...>;

	/**
	 * \brief Convenience alias that reads as "all expressions valid for any cvref".
	 */
	template <class ExprList, class... Ts> inline constexpr bool all_exprs_valid_for_any_cvref_v = all_exprs_detect_any_cvref_v<ExprList, Ts...>;

	/**
	 * \brief Convenience alias that reads as "any expression valid for any cvref".
	 */
	template <class ExprList, class... Ts> inline constexpr bool any_expr_valid_for_any_cvref_v = any_expr_detect_any_cvref_v<ExprList, Ts...>;

	/**
	 * \brief Catalog of operator and library expression templates.
	 */
	namespace op
	{
		/**
		 * \brief Expression for unary `+t` on lvalues.
		 */
		template <class T> using unary_plus = decltype(+std::declval<T&>());

		/**
		 * \brief Expression for unary `-t` on lvalues.
		 */
		template <class T> using negate = decltype(-std::declval<T&>());

		/**
		 * \brief Expression for unary `~t` on lvalues.
		 */
		template <class T> using bit_not = decltype(~std::declval<T&>());

		/**
		 * \brief Expression for unary `!t` on lvalues.
		 */
		template <class T> using logical_not = decltype(!std::declval<T&>());

		/**
		 * \brief Expression for unary `*t` on lvalues.
		 */
		template <class T> using dereference = decltype(*std::declval<T&>());

		/**
		 * \brief Expression for unary `&t` on lvalues.
		 */
		template <class T> using address_of = decltype(&std::declval<T&>());

		/**
		 * \brief Expression for pre-increment `++t` on lvalues.
		 */
		template <class T> using pre_increment = decltype(++std::declval<T&>());

		/**
		 * \brief Expression for post-increment `t++` on lvalues.
		 */
		template <class T> using post_increment = decltype(std::declval<T&>()++);

		/**
		 * \brief Expression for pre-decrement `--t` on lvalues.
		 */
		template <class T> using pre_decrement = decltype(--std::declval<T&>());

		/**
		 * \brief Expression for post-decrement `t--` on lvalues.
		 */
		template <class T> using post_decrement = decltype(std::declval<T&>()--);

		/**
		 * \brief Expression for unary `+t` on forwarded operands.
		 */
		template <class T> using unary_plus_fwd = decltype(+std::declval<T>());

		/**
		 * \brief Expression for unary `-t` on forwarded operands.
		 */
		template <class T> using negate_fwd = decltype(-std::declval<T>());

		/**
		 * \brief Expression for unary `~t` on forwarded operands.
		 */
		template <class T> using bit_not_fwd = decltype(~std::declval<T>());

		/**
		 * \brief Expression for unary `!t` on forwarded operands.
		 */
		template <class T> using logical_not_fwd = decltype(!std::declval<T>());

		/**
		 * \brief Expression for unary `*t` on forwarded operands.
		 */
		template <class T> using dereference_fwd = decltype(*std::declval<T>());

		/**
		 * \brief Expression for `a + b` on lvalues.
		 */
		template <class T, class U> using plus = decltype(std::declval<T&>() + std::declval<U&>());

		/**
		 * \brief Expression for `a - b` on lvalues.
		 */
		template <class T, class U> using minus = decltype(std::declval<T&>() - std::declval<U&>());

		/**
		 * \brief Expression for `a * b` on lvalues.
		 */
		template <class T, class U> using multiplies = decltype(std::declval<T&>() * std::declval<U&>());

		/**
		 * \brief Expression for `a / b` on lvalues.
		 */
		template <class T, class U> using divides = decltype(std::declval<T&>() / std::declval<U&>());

		/**
		 * \brief Expression for `a % b` on lvalues.
		 */
		template <class T, class U> using modulus = decltype(std::declval<T&>() % std::declval<U&>());

		/**
		 * \brief Expression for `a & b` on lvalues.
		 */
		template <class T, class U> using bit_and = decltype(std::declval<T&>() & std::declval<U&>());

		/**
		 * \brief Expression for `a | b` on lvalues.
		 */
		template <class T, class U> using bit_or = decltype(std::declval<T&>() | std::declval<U&>());

		/**
		 * \brief Expression for `a ^ b` on lvalues.
		 */
		template <class T, class U> using bit_xor = decltype(std::declval<T&>() ^ std::declval<U&>());

		/**
		 * \brief Expression for `a << b` on lvalues.
		 */
		template <class T, class U> using shift_left = decltype(std::declval<T&>() << std::declval<U&>());

		/**
		 * \brief Expression for `a >> b` on lvalues.
		 */
		template <class T, class U> using shift_right = decltype(std::declval<T&>() >> std::declval<U&>());

		/**
		 * \brief Expression for `a << b` on forwarded operands.
		 */
		template <class T, class U> using shift_left_fwd = decltype(std::declval<T>() << std::declval<U>());

		/**
		 * \brief Expression for `a >> b` on forwarded operands.
		 */
		template <class T, class U> using shift_right_fwd = decltype(std::declval<T>() >> std::declval<U>());

		/**
		 * \brief Expression for calling `f(args...)` on an lvalue callable.
		 */
		template <class F, class... A> using call_lvalue = decltype(std::declval<F&>()(std::declval<A>()...));

		/**
		 * \brief Expression for calling `f(args...)` on a forwarded callable.
		 */
		template <class F, class... A> using call_fwd = decltype(std::declval<F>()(std::declval<A>()...));

		/**
		 * \brief Alias for the forwarded call probe.
		 */
		template <class F, class... A> using call = call_fwd<F, A...>;

		/**
		 * \brief Expression for `std::invoke(f, args...)` on forwarded operands.
		 */
		template <class F, class... A> using invoke_fwd = decltype(std::invoke(std::declval<F>(), std::declval<A>()...));

		/**
		 * \brief Expression for `a == b` on const lvalues.
		 */
		template <class T, class U> using equal_to = decltype(std::declval<T const&>() == std::declval<U const&>());

		/**
		 * \brief Expression for `a != b` on const lvalues.
		 */
		template <class T, class U> using not_equal_to = decltype(std::declval<T const&>() != std::declval<U const&>());

		/**
		 * \brief Expression for `a < b` on const lvalues.
		 */
		template <class T, class U> using less = decltype(std::declval<T const&>() < std::declval<U const&>());

		/**
		 * \brief Expression for `a <= b` on const lvalues.
		 */
		template <class T, class U> using less_equal = decltype(std::declval<T const&>() <= std::declval<U const&>());

		/**
		 * \brief Expression for `a > b` on const lvalues.
		 */
		template <class T, class U> using greater = decltype(std::declval<T const&>() > std::declval<U const&>());

		/**
		 * \brief Expression for `a >= b` on const lvalues.
		 */
		template <class T, class U> using greater_equal = decltype(std::declval<T const&>() >= std::declval<U const&>());

		/**
		 * \brief Expression for `a == b` on forwarded operands.
		 */
		template <class T, class U> using equal_to_fwd = decltype(std::declval<T>() == std::declval<U>());

		/**
		 * \brief Expression for `a != b` on forwarded operands.
		 */
		template <class T, class U> using not_equal_to_fwd = decltype(std::declval<T>() != std::declval<U>());

		/**
		 * \brief Expression for `a < b` on forwarded operands.
		 */
		template <class T, class U> using less_fwd = decltype(std::declval<T>() < std::declval<U>());

		/**
		 * \brief Expression for `a <= b` on forwarded operands.
		 */
		template <class T, class U> using less_equal_fwd = decltype(std::declval<T>() <= std::declval<U>());

		/**
		 * \brief Expression for `a > b` on forwarded operands.
		 */
		template <class T, class U> using greater_fwd = decltype(std::declval<T>() > std::declval<U>());

		/**
		 * \brief Expression for `a >= b` on forwarded operands.
		 */
		template <class T, class U> using greater_equal_fwd = decltype(std::declval<T>() >= std::declval<U>());

		/**
		 * \brief Expression for `a && b` on lvalues.
		 */
		template <class T, class U> using logical_and = decltype(std::declval<T&>() && std::declval<U&>());

		/**
		 * \brief Expression for `a || b` on lvalues.
		 */
		template <class T, class U> using logical_or = decltype(std::declval<T&>() || std::declval<U&>());

		/**
		 * \brief Expression for `a && b` on forwarded operands.
		 */
		template <class T, class U> using logical_and_fwd = decltype(std::declval<T>() && std::declval<U>());

		/**
		 * \brief Expression for `a || b` on forwarded operands.
		 */
		template <class T, class U> using logical_or_fwd = decltype(std::declval<T>() || std::declval<U>());

		/**
		 * \brief Expression for assignment `a = b`.
		 */
		template <class T, class U> using assign = decltype(std::declval<T&>() = std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a += b`.
		 */
		template <class T, class U> using plus_assign = decltype(std::declval<T&>() += std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a -= b`.
		 */
		template <class T, class U> using minus_assign = decltype(std::declval<T&>() -= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a *= b`.
		 */
		template <class T, class U> using multiplies_assign = decltype(std::declval<T&>() *= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a /= b`.
		 */
		template <class T, class U> using divides_assign = decltype(std::declval<T&>() /= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a %= b`.
		 */
		template <class T, class U> using modulus_assign = decltype(std::declval<T&>() %= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a &= b`.
		 */
		template <class T, class U> using bit_and_assign = decltype(std::declval<T&>() &= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a |= b`.
		 */
		template <class T, class U> using bit_or_assign = decltype(std::declval<T&>() |= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a ^= b`.
		 */
		template <class T, class U> using bit_xor_assign = decltype(std::declval<T&>() ^= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a <<= b`.
		 */
		template <class T, class U> using shift_left_assign = decltype(std::declval<T&>() <<= std::declval<U>());

		/**
		 * \brief Expression for compound assignment `a >>= b`.
		 */
		template <class T, class U> using shift_right_assign = decltype(std::declval<T&>() >>= std::declval<U>());

		/**
		 * \brief Expression for the comma operator `(a, b)`.
		 */
		template <class T, class U> using comma = decltype((std::declval<T>(), std::declval<U>()));

		/**
		 * \brief Expression for `c[i]` on non-const lvalues.
		 */
		template <class C, class I> using subscript = decltype(std::declval<C&>()[std::declval<I>()]);

		/**
		 * \brief Expression for `c[i]` on const lvalues.
		 */
		template <class C, class I> using subscript_const = decltype(std::declval<C const&>()[std::declval<I>()]);

		/**
		 * \brief Expression for `c[i]` on forwarded receivers.
		 */
		template <class C, class I> using subscript_fwd = decltype(std::declval<C>()[std::declval<I>()]);

		/**
		 * \brief Expression for `static_cast<bool>(t)` on lvalues.
		 */
		template <class T> using explicit_bool = decltype(static_cast<bool>(std::declval<T&>()));

		/**
		 * \brief Expression for contextual bool e.g. `t ? 0 : 1` on lvalues.
		 */
		template <class T> using contextual_bool = decltype(std::declval<T&>() ? 0 : 1);

		/**
		 * \brief Expression for `static_cast<To>(from)`.
		 */
		template <class To, class From> using static_cast_to = decltype(static_cast<To>(std::declval<From>()));

		/**
		 * \brief Expression for `const_cast<To>(from)`.
		 */
		template <class To, class From> using const_cast_to = decltype(const_cast<To>(std::declval<From>()));

		/**
		 * \brief Expression for `reinterpret_cast<To>(from)`.
		 */
		template <class To, class From> using reinterpret_cast_to = decltype(reinterpret_cast<To>(std::declval<From>()));

		/**
		 * \brief Expression for functional construction `To(from)`.
		 */
		template <class To, class From> using functional_cast_to = decltype(To(std::declval<From>()));

		/**
		 * \brief Expression for list construction `To{from}`.
		 */
		template <class To, class From> using list_init_to = decltype(To{ std::declval<From>() });

		/**
		 * \brief Expression for `dynamic_cast<To>(from)`.
		 */
		template <class To, class From> using dynamic_cast_to = decltype(dynamic_cast<To>(std::declval<From>()));

		/**
		 * \brief Expression for `sizeof(T)`.
		 */
		template <class T> using sizeof_type = decltype(sizeof(T));

		/**
		 * \brief Expression for `alignof(T)`.
		 */
		template <class T> using alignof_type = decltype(alignof(T));

		/**
		 * \brief Expression for `new T(args...)`.
		 */
		template <class T, class... A> using new_object = decltype(::new T(std::declval<A>()...));

		/**
		 * \brief Expression for `new (std::nothrow) T(args...)`.
		 */
		template <class T, class... A> using new_object_nothrow = decltype(::new (std::nothrow) T(std::declval<A>()...));

		/**
		 * \brief Expression for placement-new `new (ptr) T(args...)`.
		 */
		template <class T, class... A> using placement_new_object = decltype(::new (std::declval<void*>()) T(std::declval<A>()...));

		/**
		 * \brief Expression for nothrow placement-new `new (std::nothrow) T(args...)`.
		 */
		template <class T, class... A> using placement_new_object_nothrow = decltype(::new (std::nothrow) T(std::declval<A>()...));

		/**
		 * \brief Expression for `new T[n]`.
		 */
		template <class T> using new_array = decltype(::new T[std::declval<std::size_t>()]);

		/**
		 * \brief Expression for `new (std::nothrow) T[n]`.
		 */
		template <class T> using new_array_nothrow = decltype(::new (std::nothrow) T[std::declval<std::size_t>()]);

		/**
		 * \brief Expression for `delete p`.
		 */
		template <class P> using delete_object_expr = decltype((delete std::declval<P>(), 0));

		/**
		 * \brief Expression for `delete[] p`.
		 */
		template <class P> using delete_array_expr = decltype((delete[] std::declval<P>(), 0));

		/**
		 * \brief Expression for `::operator new(args...)`.
		 */
		template <class... A> using global_operator_new = decltype(::operator new(std::declval<A>()...));

		/**
		 * \brief Expression for `::operator new[](args...)`.
		 */
		template <class... A> using global_operator_new_array = decltype(::operator new[](std::declval<A>()...));

		/**
		 * \brief Expression for `::operator delete(args...)`.
		 */
		template <class... A> using global_operator_delete = decltype(::operator delete(std::declval<A>()...));

		/**
		 * \brief Expression for `::operator delete[](args...)`.
		 */
		template <class... A> using global_operator_delete_array = decltype(::operator delete[](std::declval<A>()...));

		/**
		 * \brief Expression for `T::operator new(args...)`.
		 */
		template <class T, class... A> using member_operator_new = decltype(T::operator new(std::declval<A>()...));

		/**
		 * \brief Expression for `T::operator new[](args...)`.
		 */
		template <class T, class... A> using member_operator_new_array = decltype(T::operator new[](std::declval<A>()...));

		/**
		 * \brief Expression for `T::operator delete(args...)`.
		 */
		template <class T, class... A> using member_operator_delete = decltype(T::operator delete(std::declval<A>()...));

		/**
		 * \brief Expression for `T::operator delete[](args...)`.
		 */
		template <class T, class... A> using member_operator_delete_array = decltype(T::operator delete[](std::declval<A>()...));

		/**
		 * \brief Expression for `t.begin()` on lvalues.
		 */
		template <class T> using begin_expr = decltype(std::declval<T&>().begin());

		/**
		 * \brief Expression for `t.end()` on lvalues.
		 */
		template <class T> using end_expr = decltype(std::declval<T&>().end());

		/**
		 * \brief Expression for `t.begin()` on const lvalues.
		 */
		template <class T> using cbegin_expr = decltype(std::declval<T const&>().begin());

		/**
		 * \brief Expression for `t.end()` on const lvalues.
		 */
		template <class T> using cend_expr = decltype(std::declval<T const&>().end());

		/**
		 * \brief Expression for `t.begin()` on forwarded receivers.
		 */
		template <class T> using begin_expr_fwd = decltype(std::declval<T>().begin());

		/**
		 * \brief Expression for `t.end()` on forwarded receivers.
		 */
		template <class T> using end_expr_fwd = decltype(std::declval<T>().end());

		/**
		 * \brief Expression for `t.size()` on lvalues.
		 */
		template <class T> using size_expr = decltype(std::declval<T&>().size());

		/**
		 * \brief Expression for `t.size()` on const lvalues.
		 */
		template <class T> using size_expr_const = decltype(std::declval<T const&>().size());

		/**
		 * \brief Expression for `t.data()` on lvalues.
		 */
		template <class T> using data_expr = decltype(std::declval<T&>().data());

		/**
		 * \brief Expression for `t.data()` on const lvalues.
		 */
		template <class T> using data_expr_const = decltype(std::declval<T const&>().data());

		/**
		 * \brief Expression for `t.empty()` on lvalues.
		 */
		template <class T> using empty_expr = decltype(std::declval<T&>().empty());

		/**
		 * \brief Expression for `t.empty()` on const lvalues.
		 */
		template <class T> using empty_expr_const = decltype(std::declval<T const&>().empty());

		/**
		 * \brief Expression for `t.size()` on forwarded receivers.
		 */
		template <class T> using size_expr_fwd = decltype(std::declval<T>().size());

		/**
		 * \brief Expression for `t.data()` on forwarded receivers.
		 */
		template <class T> using data_expr_fwd = decltype(std::declval<T>().data());

		/**
		 * \brief Expression for `t.empty()` on forwarded receivers.
		 */
		template <class T> using empty_expr_fwd = decltype(std::declval<T>().empty());

		/**
		 * \brief Expression for `t.rbegin()` on lvalues.
		 */
		template <class T> using rbegin_expr = decltype(std::declval<T&>().rbegin());

		/**
		 * \brief Expression for `t.rend()` on lvalues.
		 */
		template <class T> using rend_expr = decltype(std::declval<T&>().rend());

		/**
		 * \brief Expression for `t.rbegin()` on const lvalues.
		 */
		template <class T> using crbegin_expr = decltype(std::declval<T const&>().rbegin());

		/**
		 * \brief Expression for `t.rend()` on const lvalues.
		 */
		template <class T> using crend_expr = decltype(std::declval<T const&>().rend());

		/**
		 * \brief Expression for `t.rbegin()` on forwarded receivers.
		 */
		template <class T> using rbegin_expr_fwd = decltype(std::declval<T>().rbegin());

		/**
		 * \brief Expression for `t.rend()` on forwarded receivers.
		 */
		template <class T> using rend_expr_fwd = decltype(std::declval<T>().rend());

		/**
		 * \brief Internal helpers for ADL probes and arrow chaining.
		 */
		namespace details
		{
			using std::begin;
			using std::cbegin;
			using std::cend;
			using std::crbegin;
			using std::crend;
			using std::data;
			using std::empty;
			using std::end;
			using std::rbegin;
			using std::rend;
			using std::size;
			using std::swap;

			/**
			 * \brief ADL-enabled `begin(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_begin(T&& value) -> decltype(begin(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `end(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_end(T&& value) -> decltype(end(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `size(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_size(T&& value) -> decltype(size(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `data(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_data(T&& value) -> decltype(data(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `empty(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_empty(T&& value) -> decltype(empty(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `rbegin(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_rbegin(T&& value) -> decltype(rbegin(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `rend(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_rend(T&& value) -> decltype(rend(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `cbegin(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_cbegin(T&& value) -> decltype(cbegin(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `cend(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_cend(T&& value) -> decltype(cend(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `crbegin(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_crbegin(T&& value) -> decltype(crbegin(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `crend(value)` probe.
			 */
			template <class T>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_crend(T&& value) -> decltype(crend(std::forward<T>(value)));

			/**
			 * \brief ADL-enabled `swap(left, right)` probe.
			 */
			template <class T, class U>
			// ReSharper disable once CppFunctionIsNotImplemented
			auto adl_swap(T&& left, U&& right) -> decltype(swap(std::forward<T>(left), std::forward<U>(right)));

			/**
			 * \brief Expression alias for `adl_begin(t)` on lvalues.
			 */
			template <class T> using adl_begin_expr = decltype(adl_begin(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_end(t)` on lvalues.
			 */
			template <class T> using adl_end_expr = decltype(adl_end(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_size(t)` on lvalues.
			 */
			template <class T> using adl_size_expr = decltype(adl_size(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_data(t)` on lvalues.
			 */
			template <class T> using adl_data_expr = decltype(adl_data(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_empty(t)` on lvalues.
			 */
			template <class T> using adl_empty_expr = decltype(adl_empty(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_rbegin(t)` on lvalues.
			 */
			template <class T> using adl_rbegin_expr = decltype(adl_rbegin(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_rend(t)` on lvalues.
			 */
			template <class T> using adl_rend_expr = decltype(adl_rend(std::declval<T&>()));

			/**
			 * \brief Expression alias for `adl_begin(t)` on forwarded receivers.
			 */
			template <class T> using adl_begin_fwd_expr = decltype(adl_begin(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_end(t)` on forwarded receivers.
			 */
			template <class T> using adl_end_fwd_expr = decltype(adl_end(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_size(t)` on forwarded receivers.
			 */
			template <class T> using adl_size_fwd_expr = decltype(adl_size(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_data(t)` on forwarded receivers.
			 */
			template <class T> using adl_data_fwd_expr = decltype(adl_data(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_empty(t)` on forwarded receivers.
			 */
			template <class T> using adl_empty_fwd_expr = decltype(adl_empty(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_rbegin(t)` on forwarded receivers.
			 */
			template <class T> using adl_rbegin_fwd_expr = decltype(adl_rbegin(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_rend(t)` on forwarded receivers.
			 */
			template <class T> using adl_rend_fwd_expr = decltype(adl_rend(std::declval<T>()));

			/**
			 * \brief Expression alias for `adl_swap(a, b)`.
			 */
			template <class T, class U> using adl_swap_expr = decltype(adl_swap(std::declval<T>(), std::declval<U>()));

			/**
			 * \brief Removes cv/ref to form an object type.
			 */
			template <class T> using decay_t = std::remove_cv_t<std::remove_reference_t<T>>;

			/**
			 * \brief Expression for `t.operator->()`.
			 */
			template <class T> using arrow_step_t = decltype(std::declval<T&>().operator->());

			/**
			 * \brief Arrow chain recursion implementation.
			 */
			template <class T, std::size_t Depth, bool /*IsPtr*/, bool /*HasStep*/, bool CanRecurse = (Depth != 0)> struct arrow_chain_impl
			{
				using type = ::SNAP_NAMESPACE::nonesuch;
			};

			/**
			 * \brief Arrow chain base case when the current type is a pointer.
			 */
			template <class T, std::size_t Depth, bool HasStep, bool CanRecurse> struct arrow_chain_impl<T, Depth, true, HasStep, CanRecurse>
			{
				using type = decay_t<T>;
			};

			/**
			 * \brief Arrow chain base case when recursion depth is exhausted.
			 */
			template <class T, std::size_t Depth, bool HasStep> struct arrow_chain_impl<T, Depth, false, HasStep, false>
			{
				using type = ::SNAP_NAMESPACE::nonesuch;
			};

			/**
			 * \brief Conditional recursion step for arrow chaining.
			 */
			template <class Next, std::size_t NextDepth, bool NextIsPtr, bool NextHasStep, bool Continue> struct arrow_chain_next
			{
				using type = ::SNAP_NAMESPACE::nonesuch;
			};

			/**
			 * \brief Continue case for arrow chaining.
			 */
			template <class Next, std::size_t NextDepth, bool NextIsPtr, bool NextHasStep>
			struct arrow_chain_next<Next, NextDepth, NextIsPtr, NextHasStep, true>
			{
				using type = typename arrow_chain_impl<Next, NextDepth, NextIsPtr, NextHasStep>::type;
			};

			/**
			 * \brief Arrow chain base case when no step is available.
			 */
			template <class T, std::size_t Depth> struct arrow_chain_impl<T, Depth, false, false, true>
			{
				using type = ::SNAP_NAMESPACE::nonesuch;
			};

			/**
			 * \brief Arrow chain step case when `operator->()` is available.
			 */
			template <class T, std::size_t Depth> struct arrow_chain_impl<T, Depth, false, true, true>
			{
				using U		   = decay_t<T>;
				using step_raw = ::SNAP_NAMESPACE::detected_t<arrow_step_t, U>;
				using step_t   = decay_t<step_raw>;

				static constexpr bool step_is_ptr		   = std::is_pointer_v<step_t>;
				static constexpr bool step_has_step		   = ::SNAP_NAMESPACE::is_detected_v<arrow_step_t, step_t>;
				static constexpr bool step_same_as_current = std::is_same_v<step_t, U>;
				static constexpr bool continue_chain	   = !step_same_as_current;

				using type = typename arrow_chain_next<step_t, Depth - 1, step_is_ptr, step_has_step, continue_chain>::type;
			};

			/**
			 * \brief Arrow chain entry point.
			 */
			template <class T, std::size_t Depth> struct arrow_chain
			{
				using U						   = decay_t<T>;
				static constexpr bool is_ptr   = std::is_pointer_v<U>;
				static constexpr bool has_step = ::SNAP_NAMESPACE::is_detected_v<arrow_step_t, U>;
				using type					   = typename arrow_chain_impl<U, Depth, is_ptr, has_step>::type;
			};

			/**
			 * \brief Arrow chain result type.
			 */
			template <class T, std::size_t Depth> using arrow_chain_t = typename arrow_chain<T, Depth>::type;

			/**
			 * \brief True if the arrow chain ends in a pointer.
			 */
			template <class T, std::size_t Depth> inline constexpr bool arrow_chain_is_pointer_v = std::is_pointer_v<arrow_chain_t<T, Depth>>;

			/**
			 * \brief Expression for `std::hash<T>{}(t)` using `remove_cvref_t`.
			 */
			template <class T> using hash_expr =
				decltype(std::declval<std::hash<::SNAP_NAMESPACE::remove_cvref_t<T>>&>()(std::declval<::SNAP_NAMESPACE::remove_cvref_t<T> const&>()));
		} // namespace details

		/**
		 * \brief Expression for `t.*pm`.
		 */
		template <class T, class PM> using member_pointer_access = decltype(std::declval<T&>().*std::declval<PM>());

		/**
		 * \brief Expression for `t->*pm`.
		 */
		template <class T, class PM> using pointer_to_member = decltype(std::declval<T&>()->*std::declval<PM>());

		/**
		 * \brief Expression for `t.operator->()`.
		 */
		template <class T> using member_arrow = decltype(std::declval<T&>().operator->());
	} // namespace op

	/**
	 * \brief True if `T` supports pointer-style member access.
	 *
	 * This is true for raw pointers, and for types whose `operator->()` chain
	 * eventually yields a pointer within the configured depth.
	 */
	template <class T, std::size_t Depth = 16> inline constexpr bool has_arrow =
		std::is_pointer_v<std::remove_reference_t<T>> || op::details::arrow_chain_is_pointer_v<T, Depth>;
} // namespace probe

SNAP_END_NAMESPACE
