#ifndef SNP_INCLUDE_SNAP_FIXED_STRING_HPP
#define SNP_INCLUDE_SNAP_FIXED_STRING_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

// ReSharper disable once CppUnusedIncludeDirective
#include "snap/version.hpp"

#if defined(__cpp_lib_three_way_comparison) && (__cpp_lib_three_way_comparison >= 201907L)
	#include <compare>
#endif

#if defined(__cpp_lib_containers_ranges) && (__cpp_lib_containers_ranges >= 202202L)
	#include <ranges>
#endif

#include "snap/internal/compat/constexpr.hpp" // SNAP_CONSTEVAL
#include "snap/internal/compat/explicit.hpp"  // SNAP_EXPLICIT_EXPR_ONLY

SNAP_BEGIN_NAMESPACE
#if defined(__cpp_lib_containers_ranges) && (__cpp_lib_containers_ranges >= 202202L)
using from_range_t = std::from_range_t;
inline constexpr from_range_t from_range{};
#else
struct from_range_t
{
	constexpr from_range_t() = default;
};
inline constexpr from_range_t from_range{};
#endif

namespace details
{

	template <class, class, class = void> struct is_neq_comparable : std::false_type
	{
	};
	template <class T, class U> struct is_neq_comparable<T, U, std::void_t<decltype(std::declval<const T&>() != std::declval<const U&>())>> : std::true_type
	{
	};
	template <class T, class U> inline constexpr bool is_neq_comparable_v = is_neq_comparable<T, U>::value;

} // namespace details

template <class CharT, std::size_t N> class basic_fixed_string
{
public:
	// exposition-only (public to satisfy structural type)
	CharT m_data[N + 1] = {};

	// types
	using value_type			 = CharT;
	using pointer				 = value_type*;
	using const_pointer			 = const value_type*;
	using reference				 = value_type&;
	using const_reference		 = const value_type&;
	using const_iterator		 = const value_type*;
	using iterator				 = const_iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator		 = const_reverse_iterator;
	using size_type				 = std::size_t;
	using difference_type		 = std::ptrdiff_t;

	template <class... Chars,
			  std::enable_if_t<(sizeof...(Chars) == N) && std::conjunction_v<std::is_same<Chars, CharT>...> &&
								   std::conjunction_v<std::bool_constant<!std::is_pointer_v<Chars>>...>,
							   int> = 0>
	constexpr explicit basic_fixed_string(Chars... chars) noexcept : m_data{ static_cast<CharT>(chars)..., CharT{} }
	{
	}

	SNAP_CONSTEVAL SNAP_EXPLICIT_EXPR_ONLY(false) basic_fixed_string(const CharT (&txt)[N + 1]) noexcept
	{
		assert(txt[N] == CharT{});
		for (std::size_t i = 0; i < N; ++i) m_data[i] = txt[i];
	}

	template <class It, class S> constexpr explicit basic_fixed_string(It begin, S end)
	{
		using cat = typename std::iterator_traits<It>::iterator_category;
		using val = typename std::iterator_traits<It>::value_type;

		static_assert(std::is_base_of_v<std::input_iterator_tag, cat>, "It must model an input iterator (C++17 tag check)");
		static_assert(std::is_same_v<std::remove_cv_t<val>, CharT>, "iterator value_type must be CharT");
		static_assert(details::is_neq_comparable_v<It, S>, "begin and end must be comparable with !=");

		auto tmp		  = begin;
		std::size_t count = 0;
		for (; tmp != end; ++tmp) ++count;

		assert(count == N);

		// Copy exactly N elements and null-terminate.
		CharT* out = m_data;
		for (; begin != end; ++begin, ++out) *out = static_cast<CharT>(*begin);
		m_data[N] = CharT{};
	}

#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L

	template <class R, std::enable_if_t<std::ranges::input_range<R> && std::is_same_v<std::ranges::range_value_t<R>, CharT>, int> = 0>
	constexpr basic_fixed_string(std::from_range_t, R&& r)
	{
		assert(std::ranges::size(r) == N);
		for (auto it = m_data; auto&& v : std::forward<R>(r)) { *it++ = std::forward<decltype(v)>(v); }
	}
#endif

	constexpr basic_fixed_string(const basic_fixed_string&) noexcept			= default;
	constexpr basic_fixed_string& operator=(const basic_fixed_string&) noexcept = default;

	[[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }
	[[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
	[[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }
	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

	// capacity
	static constexpr std::integral_constant<size_type, N> size{};
	static constexpr std::integral_constant<size_type, N> length{};
	static constexpr std::integral_constant<size_type, N> max_size{};
	static constexpr std::bool_constant<N == 0> empty{};

	// element access
	[[nodiscard]] constexpr const_reference operator[](size_type pos) const
	{
		assert(pos < N);
		return data()[pos];
	}

	[[nodiscard]] constexpr const_reference at(size_type pos) const
	{
		if (pos >= size()) throw std::out_of_range("basic_fixed_string::at");
		return (*this)[pos];
	}

	[[nodiscard]] constexpr const_reference front() const
	{
		static_assert(!empty());
		return (*this)[0];
	}

	[[nodiscard]] constexpr const_reference back() const
	{
		static_assert(!empty());
		return (*this)[N - 1];
	}

	// modifiers
	constexpr void swap(basic_fixed_string& s) noexcept { swap_ranges(begin(), end(), s.begin()); }

	// string operations
	[[nodiscard]] constexpr const_pointer c_str() const noexcept { return data(); }
	[[nodiscard]] constexpr const_pointer data() const noexcept { return static_cast<const_pointer>(m_data); }

	[[nodiscard]] constexpr std::basic_string_view<CharT> view() const noexcept
	{
		return std::basic_string_view<CharT>(data(), N); // was: (cbegin(), cend())
	}

	// NOLINTNEXTLINE(*-explicit-conversions, google-explicit-constructor)
	[[nodiscard]] constexpr SNAP_EXPLICIT_EXPR_ONLY(false) operator std::basic_string_view<CharT>() const noexcept { return view(); }

	template <std::size_t N2>
	[[nodiscard]] constexpr friend basic_fixed_string<CharT, N + N2> operator+(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs) noexcept
	{
		CharT txt[N + N2]{};
		CharT* it = txt;
		for (CharT c : lhs) *it++ = c;
		for (CharT c : rhs) *it++ = c;
		return basic_fixed_string<CharT, N + N2>(txt, it);
	}

	[[nodiscard]] constexpr friend basic_fixed_string<CharT, N + 1> operator+(const basic_fixed_string& lhs, CharT rhs) noexcept
	{
		CharT txt[N + 1]{};
		CharT* it = txt;
		for (CharT c : lhs) *it++ = c;
		*it++ = rhs;
		return basic_fixed_string<CharT, N + 1>(txt, it);
	}

	[[nodiscard]] constexpr friend basic_fixed_string<CharT, 1 + N> operator+(const CharT lhs, const basic_fixed_string& rhs) noexcept
	{
		CharT txt[1 + N]{};
		CharT* it = txt;
		*it++	  = lhs;
		for (CharT c : rhs) *it++ = c;
		return basic_fixed_string<CharT, 1 + N>(txt, it);
	}

	template <std::size_t N2>
	[[nodiscard]] SNAP_CONSTEVAL friend basic_fixed_string<CharT, N + N2 - 1> operator+(const basic_fixed_string& lhs, const CharT (&rhs)[N2]) noexcept
	{
		assert(rhs[N2 - 1] == CharT{});
		CharT txt[N + N2]{};
		CharT* it = txt;
		for (CharT c : lhs) *it++ = c;
		for (CharT c : rhs) *it++ = c;
		return txt;
	}

	template <std::size_t N1>
	[[nodiscard]] SNAP_CONSTEVAL friend basic_fixed_string<CharT, N1 + N - 1> operator+(const CharT (&lhs)[N1], const basic_fixed_string& rhs) noexcept
	{
		assert(lhs[N1 - 1] == CharT{});
		CharT txt[N1 + N]{};
		CharT* it = txt;
		for (std::size_t i = 0; i != N1 - 1; ++i) *it++ = lhs[i];
		for (CharT c : rhs) *it++ = c;
		*it++ = CharT();
		return txt;
	}

	// non-member comparison functions
	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator==(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return lhs.view() == rhs.view();
	}

	template <std::size_t N2> [[nodiscard]] friend SNAP_CONSTEVAL bool operator==(const basic_fixed_string& lhs, const CharT (&rhs)[N2]) noexcept
	{
		static_assert(N2 >= 1, "array must be at least 1 (null-terminated)");
		assert(rhs[N2 - 1] == CharT{});
		return lhs.view() == std::basic_string_view<CharT>(rhs, N2 - 1);
	}

#if defined(__cpp_impl_three_way_comparison) || defined(__cpp_lib_three_way_comparison)
	template <std::size_t N2> [[nodiscard]] friend constexpr auto operator<=>(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return lhs.view() <=> rhs.view();
	}

	template <std::size_t N2> [[nodiscard]] friend SNAP_CONSTEVAL auto operator<=>(const basic_fixed_string& lhs, const CharT (&rhs)[N2])
	{
		assert(rhs[N2 - 1] == CharT{});
		return lhs.view() <=> std::basic_string_view<CharT>(std::cbegin(rhs), std::cend(rhs) - 1);
	}

#else

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator!=(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator<(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return lhs.view() < rhs.view();
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator>(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return rhs < lhs;
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator<=(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return !(rhs < lhs);
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator>=(const basic_fixed_string& lhs, const basic_fixed_string<CharT, N2>& rhs)
	{
		return !(lhs < rhs);
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator!=(const basic_fixed_string& lhs, const CharT (&rhs)[N2]) { return !(lhs == rhs); }

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator<(const basic_fixed_string& lhs, const CharT (&rhs)[N2])
	{
		return lhs.view() < std::basic_string_view<CharT>(rhs, N2 - 1);
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator>(const basic_fixed_string& lhs, const CharT (&rhs)[N2])
	{
		return std::basic_string_view<CharT>(rhs, N2 - 1) < lhs.view();
	}

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator<=(const basic_fixed_string& lhs, const CharT (&rhs)[N2]) { return !(lhs > rhs); }

	template <std::size_t N2> [[nodiscard]] friend constexpr bool operator>=(const basic_fixed_string& lhs, const CharT (&rhs)[N2]) { return !(lhs < rhs); }
#endif

	// inserters and extractors
	friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const basic_fixed_string& str) { return os << str.c_str(); }
};

template <class CharT, class... Rest, std::enable_if_t<std::conjunction_v<std::is_same<Rest, CharT>...>, int> = 0> basic_fixed_string(CharT, Rest...)
	-> basic_fixed_string<CharT, 1 + sizeof...(Rest)>;

template <class CharT, std::size_t N> basic_fixed_string(const CharT (&)[N]) -> basic_fixed_string<CharT, N - 1>;

#if defined(__cpp_lib_containers_ranges) && (__cpp_lib_containers_ranges >= 202202L)
template <class CharT, std::size_t N> basic_fixed_string(std::from_range_t, std::array<CharT, N>) -> basic_fixed_string<CharT, N>;
#endif

template <class CharT, std::size_t N> constexpr void swap(basic_fixed_string<CharT, N>& x, basic_fixed_string<CharT, N>& y) noexcept
{
	x.swap(y);
}

template <std::size_t N> using fixed_string = basic_fixed_string<char, N>;

#ifdef __cpp_char8_t // C++20 or newer provides char8_t
template <std::size_t N> using fixed_u8string = basic_fixed_string<char8_t, N>;
#endif

template <std::size_t N> using fixed_u16string = basic_fixed_string<char16_t, N>;
template <std::size_t N> using fixed_u32string = basic_fixed_string<char32_t, N>;
template <std::size_t N> using fixed_wstring   = basic_fixed_string<wchar_t, N>;

SNAP_END_NAMESPACE

// std::hash
#include <functional>

template <std::size_t N> struct std::hash<SNAP_NAMESPACE::fixed_string<N>> : std::hash<std::string_view>
{
};

#ifdef __cpp_char8_t
	#include <string_view> // for std::u8string_view
template <std::size_t N> struct std::hash<SNAP_NAMESPACE::fixed_u8string<N>> : std::hash<std::u8string_view>
{
};
#endif

template <std::size_t N> struct std::hash<SNAP_NAMESPACE::fixed_u16string<N>> : std::hash<std::u16string_view>
{
};
template <std::size_t N> struct std::hash<SNAP_NAMESPACE::fixed_u32string<N>> : std::hash<std::u32string_view>
{
};
template <std::size_t N> struct std::hash<SNAP_NAMESPACE::fixed_wstring<N>> : std::hash<std::wstring_view>
{
};

// format
#if defined(__has_include)
	#if __has_include(<format>)
		#include <format>
		#if defined(__cpp_lib_format)
namespace std
{
	template <class CharT, std::size_t N, class ParseCharT> struct formatter<SNAP_NAMESPACE::basic_fixed_string<CharT, N>, ParseCharT>
		: formatter<std::basic_string_view<CharT>, ParseCharT>
	{
		template <class FormatContext> auto format(const SNAP_NAMESPACE::basic_fixed_string<CharT, N>& str, FormatContext& ctx) const -> decltype(ctx.out())
		{
			using SV = std::basic_string_view<CharT>;
			return formatter<SV, ParseCharT>::format(SV(str), ctx);
		}
	};
} // namespace std
		#endif
	#endif
#endif

#endif // SNP_INCLUDE_SNAP_FIXED_STRING_HPP
