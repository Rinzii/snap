#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/has.hpp"

SNAP_BEGIN_NAMESPACE

/**
 * \brief High-level boolean probes built on top of `probe::*`.
 *
 * These are convenience predicates that combine one or more low-level expression checks
 * into a single `*_v` boolean.
 */
namespace meta::traits
{
	/**
	 * \brief True if `T` and `U` support `==` and `!=` in at least one cvref form.
	 */
	template <class T, class U>
	inline constexpr bool is_equality_comparable_v =
		probe::all_exprs_detect_any_cvref_v<
			probe::all_of<
				probe::op::equal_to_fwd,
				probe::op::not_equal_to_fwd>,
			T,
			U>;

	/**
	 * \brief True if `T` and `U` support `<, <=, >, >=` in at least one cvref form.
	 */
	template <class T, class U>
	inline constexpr bool is_relational_comparable_v =
		probe::all_exprs_detect_any_cvref_v<
			probe::all_of<
				probe::op::less_fwd,
				probe::op::less_equal_fwd,
				probe::op::greater_fwd,
				probe::op::greater_equal_fwd>,
			T,
			U>;

	/**
	 * \brief True if `T` and `U` are both equality comparable and relationally comparable.
	 */
	template <class T, class U>
	inline constexpr bool is_totally_ordered_v =
		is_equality_comparable_v<T, U> && is_relational_comparable_v<T, U>;

	/**
	 * \brief True if `T` and `U` support `&&` and `||` in at least one cvref form.
	 */
	template <class T, class U>
	inline constexpr bool is_logical_combinable_v =
		probe::all_exprs_detect_any_cvref_v<
			probe::all_of<
				probe::op::logical_and_fwd,
				probe::op::logical_or_fwd>,
			T,
			U>;

	/**
	 * \brief True if `T = U` is a valid assignment expression.
	 */
	template <class T, class U>
	inline constexpr bool is_assignable_v =
		probe::has<probe::op::assign, T, U>;

	/**
	 * \brief True if `T` supports the common compound assignments with `U`.
	 *
	 * Checks `+=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=` for validity.
	 */
	template <class T, class U>
	inline constexpr bool is_compound_assignable_v =
		probe::has_all<
			probe::all_of<
				probe::op::plus_assign,
				probe::op::minus_assign,
				probe::op::multiplies_assign,
				probe::op::divides_assign,
				probe::op::modulus_assign,
				probe::op::bit_and_assign,
				probe::op::bit_or_assign,
				probe::op::bit_xor_assign,
				probe::op::shift_left_assign,
				probe::op::shift_right_assign>,
			T,
			U>;

	/**
	 * \brief True if `*t` is valid for at least one value category form.
	 */
	template <class T>
	inline constexpr bool is_dereferenceable_v =
		probe::has<probe::op::dereference, T> || probe::has<probe::op::dereference_fwd, T>;

	/**
	 * \brief True if both pre-increment and post-increment are valid.
	 */
	template <class T>
	inline constexpr bool is_incrementable_v =
		probe::has<probe::op::pre_increment, T> && probe::has<probe::op::post_increment, T>;

	/**
	 * \brief True if both pre-decrement and post-decrement are valid.
	 */
	template <class T>
	inline constexpr bool is_decrementable_v =
		probe::has<probe::op::pre_decrement, T> && probe::has<probe::op::post_decrement, T>;

	/**
	 * \brief True if `F(Args...)` is a valid expression for the forwarded form of `F`.
	 */
	template <class F, class... A>
	inline constexpr bool is_callable_as_v =
		probe::has<probe::op::call_fwd, F, A...>;

	/**
	 * \brief True if `std::invoke(F, Args...)` is a valid expression for the forwarded form of `F`.
	 */
	template <class F, class... A>
	inline constexpr bool is_invocable_as_v =
		probe::has<probe::op::invoke_fwd, F, A...>;

	/**
	 * \brief True if `F` can be called with `Args...` when `F` is treated as an lvalue.
	 *
	 * Checks both direct call syntax and `std::invoke`.
	 */
	template <class F, class... A>
	inline constexpr bool is_callable_lvalue_v =
		is_callable_as_v<F&, A...> || is_callable_as_v<F const&, A...> ||
		is_invocable_as_v<F&, A...> || is_invocable_as_v<F const&, A...>;

	/**
	 * \brief True if `F` can be called with `Args...` when `F` is treated as an rvalue.
	 *
	 * Checks both direct call syntax and `std::invoke`.
	 */
	template <class F, class... A>
	inline constexpr bool is_callable_rvalue_v =
		is_callable_as_v<F&&, A...> || is_callable_as_v<F const&&, A...> ||
		is_invocable_as_v<F&&, A...> || is_invocable_as_v<F const&&, A...>;

	/**
	 * \brief True if `F` is callable as either an lvalue or rvalue.
	 */
	template <class F, class... A>
	inline constexpr bool is_callable_any_v =
		is_callable_lvalue_v<F, A...> || is_callable_rvalue_v<F, A...>;

	/**
	 * \brief True if `os << t` is valid for the given `Stream` and `T`.
	 */
	template <class Stream, class T>
	inline constexpr bool is_stream_insertable_as_v =
		probe::has<probe::op::shift_left_fwd, Stream&, T>;

	/**
	 * \brief True if `T` can be stream-inserted in any common cvref form.
	 */
	template <class Stream, class T>
	inline constexpr bool is_stream_insertable_v =
		is_stream_insertable_as_v<Stream, ::SNAP_NAMESPACE::remove_cvref_t<T> const&> ||
		is_stream_insertable_as_v<Stream, ::SNAP_NAMESPACE::remove_cvref_t<T>&> ||
		is_stream_insertable_as_v<Stream, ::SNAP_NAMESPACE::remove_cvref_t<T>&&>;

	/**
	 * \brief True if `static_cast<To>(from)` is valid for at least one cvref form of `From`.
	 */
	template <class To, class From>
	inline constexpr bool is_static_castable_v =
		probe::detect_any_form_v<
			probe::op::static_cast_to,
			probe::form::fixed<To>,
			probe::form::any_cvref<From>>;

	/**
	 * \brief True if `To(from)` is valid for at least one cvref form of `From`.
	 */
	template <class To, class From>
	inline constexpr bool is_functional_castable_v =
		probe::detect_any_form_v<
			probe::op::functional_cast_to,
			probe::form::fixed<To>,
			probe::form::any_cvref<From>>;

	/**
	 * \brief True if `To{from}` is valid for at least one cvref form of `From`.
	 */
	template <class To, class From>
	inline constexpr bool is_list_initializable_v =
		probe::detect_any_form_v<
			probe::op::list_init_to,
			probe::form::fixed<To>,
			probe::form::any_cvref<From>>;

	/**
	 * \brief True if `c[i]` is valid for at least one common access form.
	 */
	template <class C, class I>
	inline constexpr bool is_indexable_v =
		probe::has<probe::op::subscript, C, I> ||
		probe::has<probe::op::subscript_const, C, I> ||
		probe::has<probe::op::subscript_fwd, C, I>;

	/**
	 * \brief True if `t.begin()`/`t.end()` are available in at least one common form.
	 */
	template <class T>
	inline constexpr bool has_member_begin_end_v =
		(probe::has<probe::op::begin_expr, T> && probe::has<probe::op::end_expr, T>) ||
		(probe::has<probe::op::cbegin_expr, T> && probe::has<probe::op::cend_expr, T>) ||
		(probe::has<probe::op::begin_expr_fwd, T> && probe::has<probe::op::end_expr_fwd, T>);

	/**
	 * \brief True if `begin(t)`/`end(t)` are found via ADL in at least one common form.
	 */
	template <class T>
	inline constexpr bool has_adl_begin_end_v =
		(probe::has<probe::op::details::adl_begin_expr, T> && probe::has<probe::op::details::adl_end_expr, T>) ||
		(probe::has<probe::op::details::adl_begin_fwd_expr, T> && probe::has<probe::op::details::adl_end_fwd_expr, T>);

	/**
	 * \brief True if `T` looks like a range by member begin/end or ADL begin/end.
	 */
	template <class T>
	inline constexpr bool is_range_v =
		has_member_begin_end_v<T> || has_adl_begin_end_v<T>;

	/**
	 * \brief True if `T` is a range and also provides a size operation (member or ADL).
	 */
	template <class T>
	inline constexpr bool is_sized_range_v =
		(has_member_begin_end_v<T> &&
		 (probe::has<probe::op::size_expr, T> || probe::has<probe::op::size_expr_const, T> || probe::has<probe::op::size_expr_fwd, T>)) ||
		(has_adl_begin_end_v<T> &&
		 (probe::has<probe::op::details::adl_size_expr, T> || probe::has<probe::op::details::adl_size_fwd_expr, T>));

	/**
	 * \brief True if `T` is a range and also provides a data operation (member or ADL).
	 */
	template <class T>
	inline constexpr bool is_contiguous_range_like_v =
		is_range_v<T> &&
		(probe::has<probe::op::data_expr, T> || probe::has<probe::op::data_expr_const, T> || probe::has<probe::op::data_expr_fwd, T> ||
		 probe::has<probe::op::details::adl_data_expr, T> || probe::has<probe::op::details::adl_data_fwd_expr, T>);

	/**
	 * \brief True if `swap(t, u)` is available via ADL in both directions.
	 */
	template <class T, class U>
	inline constexpr bool is_swappable_with_v =
		probe::has<probe::op::details::adl_swap_expr, T, U> &&
		probe::has<probe::op::details::adl_swap_expr, U, T>;

	/**
	 * \brief True if `swap(t, t)` is available via ADL.
	 */
	template <class T>
	inline constexpr bool is_swappable_v =
		is_swappable_with_v<T&, T&>;

	/**
	 * \brief True if `std::hash<T>` can be called and yields a `std::size_t`.
	 */
	template <class T>
	inline constexpr bool is_hashable_v =
		probe::has_as<probe::op::details::hash_expr, std::size_t, T>;
} // namespace meta::traits

SNAP_END_NAMESPACE
