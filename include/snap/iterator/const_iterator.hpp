#pragma once

namespace snap
{
	namespace details
	{
		template <class I, class = void> struct deref_type
		{
		};
		template <class I> struct deref_type<I, std::void_t<decltype(*std::declval<I&>())>>
		{
			using type = decltype(*std::declval<I&>());
		};
		template <class I> using iter_ref_t = typename deref_type<I>::type;

		template <class I, class = void> struct is_const_lvalue_deref : std::false_type
		{
		};
		template <class I> struct is_const_lvalue_deref<I, std::void_t<iter_ref_t<I>>>
			: std::integral_constant<bool, std::is_lvalue_reference_v<iter_ref_t<I>> && std::is_const_v<std::remove_reference_t<iter_ref_t<I>>>>
		{
		};
		template <class I> constexpr bool is_const_lvalue_deref_v = is_const_lvalue_deref<I>::value;

		template <class T, class = void> struct is_iterator_like : std::false_type
		{
		};
		template <class T> struct is_iterator_like<T, std::void_t<typename std::iterator_traits<T>::value_type>> : std::true_type
		{
		};
		template <class T> constexpr bool is_iterator_like_v = is_iterator_like<T>::value;

		template <class I> using is_random_access_like					 = category_is_random_access<I>;
		template <class I> inline constexpr bool is_random_access_like_v = is_random_access_like<I>::value;

		template <class I> using is_bidirectional_like					 = category_is_bidirectional<I>;
		template <class I> inline constexpr bool is_bidirectional_like_v = is_bidirectional_like<I>::value;

		template <class I, class J> using either_convertible = std::bool_constant<(std::is_convertible_v<J, I> || std::is_convertible_v<I, J>)>;
	} // namespace details

	template <class Iter> class basic_const_iterator
	{
	public:
		using iterator_type		= Iter;
		using value_type		= typename std::iterator_traits<Iter>::value_type;
		using difference_type	= typename std::iterator_traits<Iter>::difference_type;
		using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
		using iterator_concept	= std::conditional_t<
			 snap::is_contiguous_iterator_v<Iter>,
			 snap::contiguous_iterator_tag,
			 std::conditional_t<details::is_random_access_like_v<Iter>,
								std::random_access_iterator_tag,
								std::conditional_t<details::is_bidirectional_like_v<Iter>, std::bidirectional_iterator_tag, std::forward_iterator_tag>>>;

		using underlying_ref = typename std::iterator_traits<Iter>::reference;
		using reference		 = std::conditional_t<std::is_lvalue_reference_v<underlying_ref>, const std::remove_reference_t<underlying_ref>&, const value_type>;

		using pointer = std::conditional_t<std::is_reference_v<reference>, const std::remove_reference_t<reference>*, void>;

		constexpr basic_const_iterator() = default;
		constexpr explicit basic_const_iterator(Iter it) : m_current(it) {}

		template <class U, class = decltype(Iter(std::declval<U>()))> constexpr basic_const_iterator(const basic_const_iterator<U>& other)
			: m_current(other.base())
		{
		}

		template <class U, class = std::enable_if_t<std::is_convertible_v<Iter, U> && snap::details::is_const_lvalue_deref_v<U>>> constexpr operator U() const
		{
			return static_cast<U>(m_current);
		}

		template <class U, class = std::enable_if_t<std::is_convertible_v<Iter, U>>> constexpr operator snap::basic_const_iterator<U>() const
		{
			return snap::basic_const_iterator<U>(static_cast<U>(m_current));
		}

		constexpr const Iter& base() const& { return m_current; }
		constexpr Iter base() && { return std::move(m_current); }

		constexpr reference operator*() const { return static_cast<reference>(*m_current); }

		template <class R = reference, class = std::enable_if_t<std::is_reference_v<R>, int>> constexpr auto operator->() const
		{
			return std::addressof(operator*());
		}

		constexpr basic_const_iterator& operator++()
		{
			++m_current;
			return *this;
		}
		constexpr basic_const_iterator operator++(int)
		{
			basic_const_iterator tmp(*this);
			++*this;
			return tmp;
		}

		template <class I2 = Iter, class = std::enable_if_t<details::is_bidirectional_like_v<I2>, int>> constexpr basic_const_iterator& operator--()
		{
			--m_current;
			return *this;
		}
		template <class I2 = Iter, class = std::enable_if_t<details::is_bidirectional_like_v<I2>, int>> constexpr basic_const_iterator operator--(int)
		{
			basic_const_iterator tmp(*this);
			--*this;
			return tmp;
		}

		template <class I2 = Iter, class = std::enable_if_t<details::is_random_access_like_v<I2>, int>>
		constexpr basic_const_iterator& operator+=(difference_type n)
		{
			m_current += n;
			return *this;
		}
		template <class I2 = Iter, class = std::enable_if_t<details::is_random_access_like_v<I2>, int>>
		constexpr basic_const_iterator& operator-=(difference_type n)
		{
			m_current -= n;
			return *this;
		}
		template <class I2 = Iter, class = std::enable_if_t<details::is_random_access_like_v<I2>, int>> constexpr reference operator[](difference_type n) const
		{
			return *(*this + n);
		}

		template <class I> friend constexpr bool operator==(const basic_const_iterator<I>&, const basic_const_iterator<I>&);
		template <class I> friend constexpr bool operator!=(const basic_const_iterator<I>&, const basic_const_iterator<I>&);

	private:
		Iter m_current{};
	};

	template <class I> constexpr bool operator==(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return a.m_current == b.m_current;
	}
	template <class I> constexpr bool operator!=(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return !(a == b);
	}

	template <class I, class J, class = std::enable_if_t<details::either_convertible<I, J>::value, int>>
	constexpr bool operator==(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return a.base() == b.base();
	}
	template <class I, class J, class = std::enable_if_t<details::either_convertible<I, J>::value, int>>
	constexpr bool operator!=(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return !(a == b);
	}

	template <class I> constexpr bool operator==(const basic_const_iterator<I>& a, const I& b)
	{
		return a.base() == b;
	}
	template <class I> constexpr bool operator==(const I& a, const basic_const_iterator<I>& b)
	{
		return a == b.base();
	}
	template <class I> constexpr bool operator!=(const basic_const_iterator<I>& a, const I& b)
	{
		return !(a == b);
	}
	template <class I> constexpr bool operator!=(const I& a, const basic_const_iterator<I>& b)
	{
		return !(a == b);
	}

	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return a.base() < b.base();
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return b < a;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<=(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return !(b < a);
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>=(const basic_const_iterator<I>& a, const basic_const_iterator<I>& b)
	{
		return !(a < b);
	}

	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr bool operator<(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return a.base() < b.base();
	}
	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr bool operator>(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return b < a;
	}
	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr bool operator<=(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return !(b < a);
	}
	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr bool operator>=(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
	{
		return !(a < b);
	}

	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<(const basic_const_iterator<I>& a, const I& b)
	{
		return a.base() < b;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<(const I& a, const basic_const_iterator<I>& b)
	{
		return a < b.base();
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>(const basic_const_iterator<I>& a, const I& b)
	{
		return b < a;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>(const I& a, const basic_const_iterator<I>& b)
	{
		return b < a;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<=(const basic_const_iterator<I>& a, const I& b)
	{
		return !(b < a);
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator<=(const I& a, const basic_const_iterator<I>& b)
	{
		return !(a < b);
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>=(const basic_const_iterator<I>& a, const I& b)
	{
		return !(a < b);
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr bool operator>=(const I& a, const basic_const_iterator<I>& b)
	{
		return !(a < b);
	}

	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr auto operator-(const basic_const_iterator<I>& a, const basic_const_iterator<J>& b)
		-> std::common_type_t<typename std::iterator_traits<I>::difference_type, typename std::iterator_traits<J>::difference_type>
	{
		return a.base() - b.base();
	}

	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr auto operator-(const basic_const_iterator<I>& a, const J& b)
		-> std::common_type_t<typename std::iterator_traits<I>::difference_type, typename std::iterator_traits<J>::difference_type>
	{
		if constexpr (std::is_convertible_v<J, I>) { return a.base() - static_cast<I>(b); }
		else { return static_cast<J>(a.base()) - b; }
	}

	template <
		class I,
		class J,
		class = std::enable_if_t<details::either_convertible<I, J>::value && details::is_random_access_like_v<I> && details::is_random_access_like_v<J>, int>>
	constexpr auto operator-(const J& a, const basic_const_iterator<I>& b)
		-> std::common_type_t<typename std::iterator_traits<I>::difference_type, typename std::iterator_traits<J>::difference_type>
	{
		if constexpr (std::is_convertible_v<J, I>) { return static_cast<I>(a) - b.base(); }
		else { return a - static_cast<J>(b.base()); }
	}

	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr typename std::iterator_traits<I>::difference_type operator-(const basic_const_iterator<I>& a, const I& b)
	{
		return a.base() - b;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr typename std::iterator_traits<I>::difference_type operator-(const I& a, const basic_const_iterator<I>& b)
	{
		return a - b.base();
	}

	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr basic_const_iterator<I> operator+(const basic_const_iterator<I>& it, typename std::iterator_traits<I>::difference_type n)
	{
		return basic_const_iterator<I>(it.base() + n);
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr basic_const_iterator<I> operator+(typename std::iterator_traits<I>::difference_type n, const basic_const_iterator<I>& it)
	{
		return it + n;
	}
	template <class I, class = std::enable_if_t<details::is_random_access_like_v<I>, int>>
	constexpr basic_const_iterator<I> operator-(const basic_const_iterator<I>& it, typename std::iterator_traits<I>::difference_type n)
	{
		return basic_const_iterator<I>(it.base() - n);
	}

	// generic iter_move fallback
	template <class I> constexpr auto iter_move(I&& i) noexcept(noexcept(*std::forward<I>(i))) -> decltype(*std::forward<I>(i))
	{
		return *std::forward<I>(i);
	}

	// specialization for basic_const_iterator
	template <class I> constexpr auto iter_move(const basic_const_iterator<I>& it) noexcept(noexcept(*it)) -> decltype(*it)
	{
		return *it;
	}

	template <class I> using const_iterator = std::conditional_t<details::is_const_lvalue_deref_v<I>, I, basic_const_iterator<I>>;

	template <class S> using const_sentinel = std::conditional_t<details::is_iterator_like_v<S>, const_iterator<S>, S>;

	template <class I> constexpr const_iterator<I> make_const_iterator(I it)
	{
		return const_iterator<I>(it);
	}
	template <class S> constexpr const_sentinel<S> make_const_sentinel(S s)
	{
		return const_sentinel<S>(s);
	}

} // namespace snap
