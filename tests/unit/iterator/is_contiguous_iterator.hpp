#pragma once

namespace snap
{
	struct contiguous_iterator_tag : public std::random_access_iterator_tag
	{
	};

	namespace details
	{
		template <class I, class = void> struct has_iterator_category : std::false_type
		{
		};
		template <class I> struct has_iterator_category<I, std::void_t<typename std::iterator_traits<I>::iterator_category>> : std::true_type
		{
		};
		template <class I> inline constexpr bool has_iterator_category_v = has_iterator_category<I>::value;

		template <class I, bool = has_iterator_concept_v<I>> struct iter_concept_is_contiguous : std::false_type
		{
		};
		template <class I> struct iter_concept_is_contiguous<I, true> : std::is_base_of<contiguous_iterator_tag, typename I::iterator_concept>
		{
		};

		template <class I, bool = has_iterator_category_v<I>> struct category_is_random_access : std::false_type
		{
		};
		template <class I> struct category_is_random_access<I, true>
			: std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<I>::iterator_category>
		{
		};

		template <class I, bool = has_iterator_category_v<I>> struct category_is_bidirectional : std::false_type
		{
		};
		template <class I> struct category_is_bidirectional<I, true>
			: std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<I>::iterator_category>
		{
		};
	} // namespace details

	template <class I, class = void> struct is_contiguous_iterator : std::false_type
	{
	};

	template <class T> struct is_contiguous_iterator<T *, void> : std::true_type
	{
	};
	template <class T> struct is_contiguous_iterator<const T *, void> : std::true_type
	{
	};

	template <class I> struct is_contiguous_iterator<I,
													 std::void_t<typename std::iterator_traits<I>::value_type,
																 typename std::iterator_traits<I>::difference_type,
																 decltype(*std::declval<I &>()),
																 decltype(to_address(std::declval<const I &>()))>>
	{
	private:
		using value_type	= typename std::iterator_traits<I>::value_type;
		using reference		= decltype(*std::declval<I &>());
		using address_type	= decltype(to_address(std::declval<const I &>()));
		using reference_ptr = std::add_pointer_t<reference>;

	public:
		enum : bool
		{
			value = (details::iter_concept_is_contiguous<I>::value || details::category_is_random_access<I>::value) && std::is_lvalue_reference_v<reference> &&
					std::is_same_v<value_type, remove_cvref_t<reference>> && std::is_same_v<address_type, reference_ptr>
		};
	};

	template <class I> constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<I>::value;
} // namespace snap
