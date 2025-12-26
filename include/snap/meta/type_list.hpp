#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/remove_cvref.hpp"

#include <cstddef>
#include <type_traits>

SNAP_BEGIN_NAMESPACE

template <class... Ts>
struct type_list
{
};

template <class T>
struct is_type_list : std::false_type
{
};

template <class... Ts>
struct is_type_list<type_list<Ts...>> : std::true_type
{
};

template <class T>
inline constexpr bool is_type_list_v = is_type_list<snap::remove_cvref_t<T>>::value;

namespace typelist
{
	namespace detail
	{
		template <class List>
		struct size_impl_unqual
		{
		};

		template <class... Ts>
		struct size_impl_unqual<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
		{
		};

		template <class List>
		struct size_impl : size_impl_unqual<snap::remove_cvref_t<List>>
		{
		};

		template <class List>
		struct empty_impl_unqual
		{
		};

		template <class... Ts>
		struct empty_impl_unqual<type_list<Ts...>> : std::bool_constant<(sizeof...(Ts) == 0u)>
		{
		};

		template <class List>
		struct empty_impl : empty_impl_unqual<snap::remove_cvref_t<List>>
		{
		};

		template <class List>
		struct front_impl_unqual
		{
		};

		template <class T0, class... Ts>
		struct front_impl_unqual<type_list<T0, Ts...>>
		{
			using type = T0;
		};

		template <class List>
		struct front_impl
		{
			using type = typename front_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <class... Ts>
		struct back_pack;

		template <class T0>
		struct back_pack<T0>
		{
			using type = T0;
		};

		template <class T0, class T1, class... Ts>
		struct back_pack<T0, T1, Ts...> : back_pack<T1, Ts...>
		{
		};

		template <class List>
		struct back_impl_unqual
		{
		};

		template <class T0, class... Ts>
		struct back_impl_unqual<type_list<T0, Ts...>>
		{
			using type = typename back_pack<T0, Ts...>::type;
		};

		template <class List>
		struct back_impl
		{
			using type = typename back_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <class List, class T>
		struct push_front_impl_unqual
		{
		};

		template <class... Ts, class T>
		struct push_front_impl_unqual<type_list<Ts...>, T>
		{
			using type = type_list<T, Ts...>;
		};

		template <class List, class T>
		struct push_front_impl
		{
			using type = typename push_front_impl_unqual<snap::remove_cvref_t<List>, T>::type;
		};

		template <class List, class T>
		struct push_back_impl_unqual
		{
		};

		template <class... Ts, class T>
		struct push_back_impl_unqual<type_list<Ts...>, T>
		{
			using type = type_list<Ts..., T>;
		};

		template <class List, class T>
		struct push_back_impl
		{
			using type = typename push_back_impl_unqual<snap::remove_cvref_t<List>, T>::type;
		};

		template <class List>
		struct pop_front_impl_unqual
		{
		};

		template <class T0, class... Ts>
		struct pop_front_impl_unqual<type_list<T0, Ts...>>
		{
			using type = type_list<Ts...>;
		};

		template <class List>
		struct pop_front_impl
		{
			using type = typename pop_front_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <class Acc, class... Ts>
		struct pop_back_accum;

		template <class... AccTs, class T0>
		struct pop_back_accum<type_list<AccTs...>, T0>
		{
			using type = type_list<AccTs...>;
		};

		template <class... AccTs, class T0, class T1, class... Rest>
		struct pop_back_accum<type_list<AccTs...>, T0, T1, Rest...> : pop_back_accum<type_list<AccTs..., T0>, T1, Rest...>
		{
		};

		template <class List>
		struct pop_back_impl_unqual
		{
		};

		template <class T0, class... Ts>
		struct pop_back_impl_unqual<type_list<T0, Ts...>>
		{
			using type = typename pop_back_accum<type_list<>, T0, Ts...>::type;
		};

		template <class List>
		struct pop_back_impl
		{
			using type = typename pop_back_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <std::size_t I, class T0, class... Ts>
		struct at_pack : at_pack<I - 1u, Ts...>
		{
		};

		template <class T0, class... Ts>
		struct at_pack<0u, T0, Ts...>
		{
			using type = T0;
		};

		template <class List, std::size_t I, class = void>
		struct at_impl_unqual
		{
		};

		template <class... Ts, std::size_t I>
		struct at_impl_unqual<type_list<Ts...>, I, std::enable_if_t<(I < sizeof...(Ts))>>
		{
			using type = typename at_pack<I, Ts...>::type;
		};

		template <class List, std::size_t I>
		struct at_impl
		{
			using type = typename at_impl_unqual<snap::remove_cvref_t<List>, I>::type;
		};

		template <class List, class T>
		struct contains_impl_unqual
		{
		};

		template <class... Ts, class T>
		struct contains_impl_unqual<type_list<Ts...>, T> : std::bool_constant<(std::is_same_v<T, Ts> || ...)>
		{
		};

		template <class List, class T>
		struct contains_impl : contains_impl_unqual<snap::remove_cvref_t<List>, T>
		{
		};

		template <class T, class... Ts>
		struct index_of_pack;

		template <class T>
		struct index_of_pack<T> : std::integral_constant<std::size_t, 0u>
		{
		};

		template <class T, class... Us>
		struct index_of_pack<T, T, Us...> : std::integral_constant<std::size_t, 0u>
		{
		};

		template <class T, class U0, class... Us>
		struct index_of_pack<T, U0, Us...> : std::integral_constant<std::size_t, 1u + index_of_pack<T, Us...>::value>
		{
		};

		template <class List, class T>
		struct index_of_impl_unqual
		{
		};

		template <class... Ts, class T>
		struct index_of_impl_unqual<type_list<Ts...>, T> : index_of_pack<T, Ts...>
		{
		};

		template <class List, class T>
		struct index_of_impl : index_of_impl_unqual<snap::remove_cvref_t<List>, T>
		{
		};

		template <class... Lists>
		struct concat_impl_unqual
		{
		};

		template <>
		struct concat_impl_unqual<>
		{
			using type = type_list<>;
		};

		template <class... Ts>
		struct concat_impl_unqual<type_list<Ts...>>
		{
			using type = type_list<Ts...>;
		};

		template <class... As, class... Bs, class... Rest>
		struct concat_impl_unqual<type_list<As...>, type_list<Bs...>, Rest...> : concat_impl_unqual<type_list<As..., Bs...>, Rest...>
		{
		};

		template <class... Lists>
		struct concat_impl
		{
			using type = typename concat_impl_unqual<snap::remove_cvref_t<Lists>...>::type;
		};

		template <class List>
		struct clear_impl_unqual
		{
		};

		template <class... Ts>
		struct clear_impl_unqual<type_list<Ts...>>
		{
			using type = type_list<>;
		};

		template <class List>
		struct clear_impl
		{
			using type = typename clear_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <class Acc, class... Ts>
		struct unique_accum;

		template <class... AccTs>
		struct unique_accum<type_list<AccTs...>>
		{
			using type = type_list<AccTs...>;
		};

		template <class... AccTs, class T0, class... Ts>
		struct unique_accum<type_list<AccTs...>, T0, Ts...>
		{
			static constexpr bool already = (std::is_same_v<T0, AccTs> || ...);
			using next_acc = std::conditional_t<already, type_list<AccTs...>, type_list<AccTs..., T0>>;
			using type = typename unique_accum<next_acc, Ts...>::type;
		};

		template <class List>
		struct unique_impl_unqual
		{
		};

		template <class... Ts>
		struct unique_impl_unqual<type_list<Ts...>>
		{
			using type = typename unique_accum<type_list<>, Ts...>::type;
		};

		template <class List>
		struct unique_impl
		{
			using type = typename unique_impl_unqual<snap::remove_cvref_t<List>>::type;
		};

		template <template <class> class F, class T, class = void>
		struct has_tt_apply : std::false_type
		{
		};

		template <template <class> class F, class T>
		struct has_tt_apply<F, T, std::void_t<F<T>>> : std::true_type
		{
		};

		template <template <class> class F, class T>
		inline constexpr bool has_tt_apply_v = has_tt_apply<F, T>::value;

		template <class List, template <class> class F, class = void>
		struct transform_impl_unqual
		{
		};

		template <class... Ts, template <class> class F>
		struct transform_impl_unqual<type_list<Ts...>, F, std::enable_if_t<(has_tt_apply_v<F, Ts> && ...)>>
		{
			using type = type_list<F<Ts>...>;
		};

		template <class List, template <class> class F>
		struct transform_impl
		{
			using type = typename transform_impl_unqual<snap::remove_cvref_t<List>, F>::type;
		};

		template <template <class> class Trait, class T, class = void>
		struct has_tt_type : std::false_type
		{
		};

		template <template <class> class Trait, class T>
		struct has_tt_type<Trait, T, std::void_t<typename Trait<T>::type>> : std::true_type
		{
		};

		template <template <class> class Trait, class T>
		inline constexpr bool has_tt_type_v = has_tt_type<Trait, T>::value;

		template <class List, template <class> class Trait, class = void>
		struct transform_type_impl_unqual
		{
		};

		template <class... Ts, template <class> class Trait>
		struct transform_type_impl_unqual<type_list<Ts...>, Trait, std::enable_if_t<(has_tt_type_v<Trait, Ts> && ...)>>
		{
			using type = type_list<typename Trait<Ts>::type...>;
		};

		template <class List, template <class> class Trait>
		struct transform_type_impl
		{
			using type = typename transform_type_impl_unqual<snap::remove_cvref_t<List>, Trait>::type;
		};

		template <template <class> class Pred, class T, class = void>
		struct has_tt_pred : std::false_type
		{
		};

		template <template <class> class Pred, class T>
		struct has_tt_pred<Pred, T, std::void_t<std::integral_constant<bool, static_cast<bool>(Pred<T>::value)>>> : std::true_type
		{
		};

		template <template <class> class Pred, class T>
		inline constexpr bool has_tt_pred_v = has_tt_pred<Pred, T>::value;

		template <template <class> class Pred, class Acc, class... Ts>
		struct filter_accum;

		template <template <class> class Pred, class... AccTs>
		struct filter_accum<Pred, type_list<AccTs...>>
		{
			using type = type_list<AccTs...>;
		};

		template <template <class> class Pred, class... AccTs, class T0, class... Ts>
		struct filter_accum<Pred, type_list<AccTs...>, T0, Ts...>
		{
			using keep_c = std::integral_constant<bool, static_cast<bool>(Pred<T0>::value)>;
			using next_acc = std::conditional_t<keep_c::value, type_list<AccTs..., T0>, type_list<AccTs...>>;
			using type = typename filter_accum<Pred, next_acc, Ts...>::type;
		};

		template <class List, template <class> class Pred, class = void>
		struct filter_impl_unqual
		{
		};

		template <class... Ts, template <class> class Pred>
		struct filter_impl_unqual<type_list<Ts...>, Pred, std::enable_if_t<(has_tt_pred_v<Pred, Ts> && ...)>>
		{
			using type = typename filter_accum<Pred, type_list<>, Ts...>::type;
		};

		template <class List, template <class> class Pred>
		struct filter_impl
		{
			using type = typename filter_impl_unqual<snap::remove_cvref_t<List>, Pred>::type;
		};
	} // namespace detail

	template <class List>
	inline constexpr std::size_t size_v = detail::size_impl<List>::value;

	template <class List>
	inline constexpr bool empty_v = detail::empty_impl<List>::value;

	template <class List>
	using front_t = typename detail::front_impl<List>::type;

	template <class List>
	using back_t = typename detail::back_impl<List>::type;

	template <class List, std::size_t I>
	using at_t = typename detail::at_impl<List, I>::type;

	template <class List, class T>
	using push_front_t = typename detail::push_front_impl<List, T>::type;

	template <class List, class T>
	using push_back_t = typename detail::push_back_impl<List, T>::type;

	template <class List>
	using pop_front_t = typename detail::pop_front_impl<List>::type;

	template <class List>
	using pop_back_t = typename detail::pop_back_impl<List>::type;

	template <class List, class T>
	inline constexpr bool contains_v = detail::contains_impl<List, T>::value;

	template <class List, class T>
	inline constexpr std::size_t index_of_v = detail::index_of_impl<List, T>::value;

	template <class List>
	inline constexpr std::size_t npos_v = size_v<List>;

	template <class List, class T>
	inline constexpr std::size_t find_v = index_of_v<List, T>;

	template <class... Lists>
	using concat_t = typename detail::concat_impl<Lists...>::type;

	template <class List>
	using clear_t = typename detail::clear_impl<List>::type;

	template <class List>
	using unique_t = typename detail::unique_impl<List>::type;

	template <class List, template <class> class F>
	using transform_t = typename detail::transform_impl<List, F>::type;

	template <class List, template <class> class Trait>
	using transform_type_t = typename detail::transform_type_impl<List, Trait>::type;

	template <class List, template <class> class Pred>
	using filter_t = typename detail::filter_impl<List, Pred>::type;

	// index_of_v<List, T> returns the first index i such that at_t<List, i> is T, or npos_v<List> if not found.
} // namespace typelist

SNAP_END_NAMESPACE