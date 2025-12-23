#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/remove_cvref.hpp"

#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
namespace internal
{

	// Copy both const and volatile qualifiers
	template <class From, class To> using copy_cv_t =
		std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,
						   std::add_const_t<std::conditional_t<std::is_volatile_v<std::remove_reference_t<From>>, std::add_volatile_t<To>, To>>,
						   std::conditional_t<std::is_volatile_v<std::remove_reference_t<From>>, std::add_volatile_t<To>, To>>;

	// Copy cv and ref-qualifiers
	template <class From, class To> using copy_cvref_t =
		std::conditional_t<std::is_lvalue_reference_v<From>,
						   std::add_lvalue_reference_t<copy_cv_t<From, To>>,
						   std::conditional_t<std::is_rvalue_reference_v<From>, std::add_rvalue_reference_t<copy_cv_t<From, To>>, copy_cv_t<From, To>>>;

	// XREF for standard compatibility
	template <class T> struct xref
	{
		template <class U> using apply = copy_cvref_t<T, U>;
	};

	// Conditional result type
	template <class T, class U> using cond_res_t = decltype(false ? std::declval<T (&)()>()() : std::declval<U (&)()>()());

	// core reference logic for lvalue/rvalue references
	template <class A, class B, class X = std::remove_reference_t<A>, class Y = std::remove_reference_t<B>> struct common_ref_impl
	{
	}; // fallback

	// lvalue/lvalue: cv-union
	template <class A, class B, class X, class Y> struct common_ref_impl<A &, B &, X, Y>
	{
		using type = cond_res_t<copy_cv_t<X, Y> &, copy_cv_t<Y, X> &>;
	};

	// SPECIAL CASES FOR (T&&, T&) and (T&, T&&) to return T& as per the C++20/23 standard
	template <typename T> struct common_ref_impl<T &&, T &, T, T>
	{
		using type = T &;
	};

	template <typename T> struct common_ref_impl<T &, T &&, T, T>
	{
		using type = T &;
	};

	// rvalue/rvalue
	template <class A, class B, class X, class Y> struct common_ref_impl<A &&, B &&, X, Y>
	{
	private:
		using C = std::remove_reference_t<typename common_ref_impl<X &, Y &>::type> &&;

	public:
		using type = std::enable_if_t<std::is_convertible_v<A &&, C> && std::is_convertible_v<B &&, C>, C>;
	};

	// rvalue/lvalue
	template <class A, class B, class X, class Y> struct common_ref_impl<A &&, B &, X, Y>
	{
	private:
		using D = typename common_ref_impl<const X &, Y &>::type;

	public:
		using type = std::enable_if_t<std::is_convertible_v<A &&, D>, D>;
	};

	// lvalue/rvalue: symmetric
	template <class A, class B, class X, class Y> struct common_ref_impl<A &, B &&, X, Y> : common_ref_impl<B &&, A &>
	{
	};

} // namespace internal

// Customization point for proxy/reference-like types
template <class, class, template <class> class, template <class> class> struct basic_common_reference
{
};

// Detection utility
namespace internal
{
	template <class, class = void> struct has_type_member : std::false_type
	{
	};

	template <class T> struct has_type_member<T, std::void_t<typename T::type>> : std::true_type
	{
	};
} // namespace internal

// SFINAE dispatch priority tag
template <int N> struct priority_tag : priority_tag<N - 1>
{
};

template <> struct priority_tag<0>
{
};

// Primary template (SFINAE fallback, no member)
template <class...> struct common_reference
{
};

template <> struct common_reference<>
{
};

template <class T> struct common_reference<T>
{
	using type = T;
};

template <class T, class U> struct common_reference<T, U>
{
private:
	// 1. Both refs, convertible: try ref impl
	template <class X = T, class Y = U, class Ref = typename internal::common_ref_impl<X, Y>::type> static Ref test(priority_tag<4>);

	// 2. basic_common_reference is well-formed
	template <class X	  = T,
			  class Y	  = U,
			  class Basic = typename basic_common_reference<SNAP_NAMESPACE::remove_cvref_t<X>,
															SNAP_NAMESPACE::remove_cvref_t<Y>,
															internal::xref<X>::template apply,
															internal::xref<Y>::template apply>::type>
	static Basic test(priority_tag<3>);

	// 3. conditional result
	template <class X = T, class Y = U, class Cond = internal::cond_res_t<X, Y>> static Cond test(priority_tag<2>);

	// 4. fallback to common_type
	template <class X = T, class Y = U, class CT = std::common_type_t<X, Y>> static CT test(priority_tag<1>);

	// 5. fallback: ill-formed (no type)
	static void test(priority_tag<0>);

	using result_t = decltype(test<T, U>(priority_tag<4>{}));

public:
	using type = result_t;
};

template <class T, class U, class V, class... Rest> struct common_reference<T, U, V, Rest...>
	: common_reference<typename common_reference<T, U>::type, V, Rest...>
{
};

template <class... Types> using common_reference_t = typename common_reference<Types...>::type;

// Detection alias for if common_reference_t exists
template <class... Types> using is_common_reference_detected = internal::has_type_member<common_reference<Types...>>;

SNAP_END_NAMESPACE
