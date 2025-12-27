#ifndef SNP_INCLUDE_SNAP_SIMD_SIMD_HPP
#define SNP_INCLUDE_SNAP_SIMD_SIMD_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/bit/has_single_bit.hpp"
#include "snap/type_traits/is_char.hpp"
#include "snap/type_traits/is_constant_evaluated.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

SNAP_BEGIN_NAMESPACE
namespace simd
{
	using simd_size_type = std::size_t;

	template <class T, class Abi> class basic_vec;
	template <std::size_t Bits, class Abi> class basic_mask;

	namespace detail
	{

		// Lightweight type-only container for a pack of types. Handy as a TMP carrier.
		template <class...> struct type_list
		{
		};

		// Concatenate two type_list<>s at compile time (forward declaration).
		template <class A, class B> struct tl_cat;

		// Concatenate packs: type_list<Xs...> + type_list<Ys...> -> type_list<Xs..., Ys...>.
		template <class... Xs, class... Ys> struct tl_cat<type_list<Xs...>, type_list<Ys...>>
		{
			using type = type_list<Xs..., Ys...>;
		};

		// Shorthand for tl_cat<A,B>::type.
		template <class A, class B> using tl_cat_t = typename tl_cat<A, B>::type;

		// Concatenate an arbitrary number of type_list<...> packs into one.
		template <class... Ls> struct tl_join; // primary template

		template <> struct tl_join<>
		{
			using type = type_list<>;
		}; // no lists -> empty

		template <class... Ts> struct tl_join<type_list<Ts...>>
		{
			using type = type_list<Ts...>;
		}; // single list -> itself

		// join first two lists, then recurse over the rest
		template <class... A, class... B, class... Rest> struct tl_join<type_list<A...>, type_list<B...>, Rest...>
		{
			using type = typename tl_join<type_list<A..., B...>, Rest...>::type;
		};

		template <class... Ls> using tl_join_t = typename tl_join<Ls...>::type; // convenience alias

		// Trait: false by default, true when V is basic_vec<...>.
		template <class V> struct is_basic_vec : std::false_type
		{
		};
		template <class U, class Abi> struct is_basic_vec<basic_vec<U, Abi>> : std::true_type
		{
		};

		// Trait: false by default, true when V is basic_mask<...>.
		template <class V> struct is_basic_mask : std::false_type
		{
		};
		template <std::size_t Bits, class Abi> struct is_basic_mask<basic_mask<Bits, Abi>> : std::true_type
		{
		};

		// True if T is a scalar type we allow in SIMD lanes (ints or floats).
		template <class T> struct is_vectorizable : std::integral_constant<bool, std::is_integral<T>::value || std::is_floating_point<T>::value>
		{
		};

		// Map a byte size to an unsigned integer type of exactly that size.
		template <std::size_t Bytes> struct integer_from_size;
		template <> struct integer_from_size<1>
		{
			using type = std::uint8_t;
		};
		template <> struct integer_from_size<2>
		{
			using type = std::uint16_t;
		};
		template <> struct integer_from_size<4>
		{
			using type = std::uint32_t;
		};
		template <> struct integer_from_size<8>
		{
			using type = std::uint64_t;
		};

		// Extract the "value" type associated with a data-parallel type.
		// For vectors it is the element type U; for masks it is an unsigned integer of size Bits.
		template <class T> struct value_type_of;
		template <class U, class Abi> struct value_type_of<basic_vec<U, Abi>>
		{
			using type = U;
		};
		template <std::size_t Bits, class Abi> struct value_type_of<basic_mask<Bits, Abi>>
		{
			using type = typename integer_from_size<Bits>::type;
		};

		// Compile-time mask element size in bytes for a given basic_mask.
		template <class V> struct mask_element_size;
		template <std::size_t Bits, class Abi> struct mask_element_size<basic_mask<Bits, Abi>> : std::integral_constant<std::size_t, Bits>
		{
		};

		// Type-level lane count for V. Uses the ABI tag (not V::size()) so it works cleanly in NTTP/SFINAE.
		template <class V> struct lanes_of; // primary; only specializations define a value

		// basic_vec<U,Abi> -> Abi::lanes_for<U>()
		template <class U, class Abi> struct lanes_of<basic_vec<U, Abi>> : std::integral_constant<std::size_t, Abi::template lanes_for<U>()>
		{
		};

		// basic_mask<Bits,Abi> -> Abi::lanes_for_mask<Bits>()  (Bits is in bytes)
		template <std::size_t Bits, class Abi> struct lanes_of<basic_mask<Bits, Abi>>
			: std::integral_constant<std::size_t, Abi::template lanes_for_mask<Bits>()>
		{
		};

		struct avx2_tag
		{ // 256-bit vectors
			template <class T> static constexpr std::size_t lanes_for()
			{
				static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "avx2_tag requires integral or floating types");
				return 32 / sizeof(T); // 256 bits / sizeof(T) bytes
			}
			template <std::size_t Bits> static constexpr std::size_t lanes_for_mask()
			{
				return 256 / (Bits * 8); // Bits is bytes; convert to bits
			}
			template <class U> static constexpr std::size_t alignment_for() { return 32; }
			template <std::size_t /*Bits*/> static constexpr std::size_t alignment_for_mask() { return 32; }
		};

// Builds the SIMD ABI registry in preference order (AVX-512, AVX2, AVX, SSE2, NEON), including only those enabled for this build.
#ifdef __AVX512F__
		// using abi_avx512 = type_list<avx512_tag>; // Commented until support
		using abi_avx512 = type_list<>;
#else
		using abi_avx512 = type_list<>;
#endif

#if defined(__AVX2__) || defined(_M_AVX2)
		using abi_avx2 = type_list<avx2_tag>;
#else
		using abi_avx2 = type_list<>;
#endif

#if defined(__AVX__) || defined(_M_AVX)
		// using abi_avx = type_list<avx_tag>; // Commented until support
		using abi_avx = type_list<>;
#else
		using abi_avx = type_list<>;
#endif

#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
		// using abi_sse2 = type_list<sse2_tag>; // Commented until support
		using abi_sse2 = type_list<>;
#else
		using abi_sse2 = type_list<>;
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
		// using abi_neon = type_list<neon_tag>; // Commented until support
		using abi_neon = type_list<>;
#else
		using abi_neon = type_list<>;
#endif

		using abi_registry = tl_join_t<abi_avx512, abi_avx2, abi_avx, abi_sse2, abi_neon>;

		// Select the first ABI tag in List whose lanes_for<T>() == N; yields void if none match.

		// Primary template; specializations below.
		template <class T, std::size_t N, class List> struct select_abi;

		// Empty list: no match.
		template <class T, std::size_t N> struct select_abi<T, N, type_list<>>
		{
			using type = void;
		};

		// Head/tail case: test Tag, otherwise try the rest.
		template <class T, std::size_t N, class Tag, class... Rest> struct select_abi<T, N, type_list<Tag, Rest...>>
		{
		private:
			static constexpr bool ok = (Tag::template lanes_for<T>() == N);
			using next				 = typename select_abi<T, N, type_list<Rest...>>::type;

		public:
			using type = std::conditional_t<ok, Tag, next>;
		};

		// Find an ABI tag that supports element type T with N lanes. If none matches, no ::type is provided.
		template <class T, std::size_t N, class = void> struct deduce_abi
		{
		};

		// Enabled when T is vectorizable and the registry contains a tag with lanes_for<T>() == N.
		// Exposes the matching tag as ::type; otherwise the primary template leaves it absent.
		template <class T, std::size_t N>
		struct deduce_abi<T, N, std::enable_if_t<is_vectorizable<T>::value && !std::is_void_v<typename select_abi<T, N, abi_registry>::type>, void>>
		{
			using type = typename select_abi<T, N, abi_registry>::type;
		};

		// ABI candidate for rebind<T,V>: keep V's lane count, change element type to T.
		// Uses lanes_of<V> to avoid dependent V::size() in NTTPs and asks deduce_abi for a tag.
		template <class T, class V> using rebind_vec_candidate_abi = typename deduce_abi<T, lanes_of<V>::value>::type;

		// ABI candidate for resize<N,V>: keep V's "value" type, change lane count to N.
		template <simd_size_type N, class V> using resize_candidate_abi = typename deduce_abi<typename value_type_of<V>::type, N>::type;

		// Primary templates: no ::type unless the constraints below are satisfied.
		template <class T, class V, class = void> struct rebind_impl
		{
		};

		template <simd_size_type N, class V, class = void> struct resize_impl
		{
		};

		// rebind_impl for vectors:
		// Enabled when deduce_abi finds a tag that supports (T, lanes_of<V>).
		// Result is basic_vec<T, Abi1>.
		template <class T, class U, class AbiV> struct rebind_impl<T, basic_vec<U, AbiV>, std::void_t<rebind_vec_candidate_abi<T, basic_vec<U, AbiV>>>>
		{
			using Abi1 = rebind_vec_candidate_abi<T, basic_vec<U, AbiV>>;
			using type = basic_vec<T, Abi1>;
		};

		// rebind_impl for masks:
		// Enabled when deduce_abi finds a tag that supports (T, lanes_of<V>) for the mask's lane count.
		// Bits for the new mask equals sizeof(T) per spec.
		template <class T, std::size_t Bits, class AbiV>
		struct rebind_impl<T, basic_mask<Bits, AbiV>, std::void_t<rebind_vec_candidate_abi<T, basic_mask<Bits, AbiV>>>>
		{
			using Abi1 = rebind_vec_candidate_abi<T, basic_mask<Bits, AbiV>>;
			using type = basic_mask<sizeof(T), Abi1>;
		};

		// resize_impl for vectors:
		// Enabled when deduce_abi finds a tag that supports (U, N).
		// Result is basic_vec<U, Abi1> with lane count N.
		template <simd_size_type N, class U, class AbiV> struct resize_impl<N, basic_vec<U, AbiV>, std::void_t<resize_candidate_abi<N, basic_vec<U, AbiV>>>>
		{
			using Abi1 = resize_candidate_abi<N, basic_vec<U, AbiV>>;
			using type = basic_vec<U, Abi1>;
		};

		// resize_impl for masks:
		// Enabled when deduce_abi finds a tag that supports (integer-from<Bits>, N).
		// Result is basic_mask<Bits, Abi1> with lane count N (Bits unchanged).
		template <simd_size_type N, std::size_t Bits, class AbiV>
		struct resize_impl<N, basic_mask<Bits, AbiV>, std::void_t<resize_candidate_abi<N, basic_mask<Bits, AbiV>>>>
		{
			using Abi1 = resize_candidate_abi<N, basic_mask<Bits, AbiV>>;
			using type = basic_mask<Bits, Abi1>;
		};

	} // namespace detail

	template <class T, class Abi> class basic_vec
	{
	public:
		using value_type = T;
		using abi_type	 = Abi;
		static constexpr std::size_t size() { return Abi::template lanes_for<T>(); }
	};

	template <std::size_t Bits, class Abi> class basic_mask
	{
	public:
		using value_type = bool;
		using abi_type	 = Abi;
		static constexpr std::size_t size() { return Abi::template lanes_for_mask<Bits>(); }
	};

	template <class T, class V, class Enable = void> struct rebind
	{
	};

	template <simd_size_type N, class V, class Enable = void> struct resize
	{
	};

	template <class T, class V> using rebind_t = typename rebind<T, V>::type;

	template <simd_size_type N, class V> using resize_t = typename resize<N, V>::type;

	template <class T, class V>
	struct rebind<T, V, std::enable_if_t<(detail::is_basic_vec<V>::value || detail::is_basic_mask<V>::value) && detail::is_vectorizable<T>::value>>
		: detail::rebind_impl<T, V>
	{
	};

	template <simd_size_type N, class V> struct resize<N, V, std::enable_if_t<(detail::is_basic_vec<V>::value || detail::is_basic_mask<V>::value)>>
		: detail::resize_impl<N, V>
	{
	};

	template <class T, class U = typename T::value_type, class Enable = void> struct alignment
	{
	}; // no ::value unless constraints met

	// mask + bool
	template <std::size_t Bits, class Abi> struct alignment<basic_mask<Bits, Abi>, bool, void>
		: std::integral_constant<std::size_t, Abi::template alignment_for_mask<Bits>()>
	{
	};

	// vec + vectorizable<U>
	template <class Elem, class Abi, class U> struct alignment<basic_vec<Elem, Abi>, U, std::enable_if_t<detail::is_vectorizable<U>::value>>
		: std::integral_constant<std::size_t, Abi::template alignment_for<U>()>
	{
	};

	template <class T, class U = typename T::value_type> constexpr std::size_t alignment_v = alignment<T, U>::value;

	template <class... Flags> struct flags;

	struct convert_flag
	{
	};
	struct aligned_flag
	{
	};
	template <std::size_t N> struct overaligned_flag
	{
		static constexpr std::size_t value = N;
	};

	namespace detail
	{

		// Pull in typelist helpers defined elsewhere.
		using ::SNAP_NAMESPACE::simd::detail::tl_cat_t;
		using ::SNAP_NAMESPACE::simd::detail::type_list;

		// Flag kind detectors: tell which flag a type is.
		template <class T> struct is_convert_flag : std::is_same<T, convert_flag>
		{
		};
		template <class T> struct is_aligned_flag : std::is_same<T, aligned_flag>
		{
		};
		template <class T> struct is_overaligned_flag : std::false_type
		{
		};
		template <std::size_t N> struct is_overaligned_flag<overaligned_flag<N>> : std::true_type
		{
		};

		// True if T is one of the supported flag types.
		template <class T> struct is_flag_kind
			: std::integral_constant<bool, is_convert_flag<T>::value || is_aligned_flag<T>::value || is_overaligned_flag<T>::value>
		{
		};

		// True if all Ts are valid flag types.
		template <class... Ts> struct all_flags : std::conjunction<is_flag_kind<Ts>...>
		{
		};

		// Does a typelist contain a convert_flag?
		template <class List> struct contains_convert;
		template <class... Ts> struct contains_convert<type_list<Ts...>> : std::disjunction<is_convert_flag<Ts>...>
		{
		};

		// Does a typelist contain an aligned_flag?
		template <class List> struct contains_aligned;
		template <class... Ts> struct contains_aligned<type_list<Ts...>> : std::disjunction<is_aligned_flag<Ts>...>
		{
		};

		// Compute the max overaligned_flag<N> value in a typelist (0 if none).
		template <class List> struct max_overaligned;
		template <> struct max_overaligned<type_list<>> : std::integral_constant<std::size_t, 0>
		{
		};
		template <class Head, class... Tail> struct max_overaligned<type_list<Head, Tail...>> : max_overaligned<type_list<Tail...>>
		{
		};
		template <std::size_t N, class... Tail> struct max_overaligned<type_list<overaligned_flag<N>, Tail...>>
		{
		private:
			static constexpr std::size_t rest = max_overaligned<type_list<Tail...>>::value;

		public:
			static constexpr std::size_t value = (N > rest ? N : rest);
		};

		// Wrap T in a typelist if K is true; otherwise an empty typelist.
		template <bool K, class T> struct maybe;
		template <class T> struct maybe<true, T>
		{
			using type = type_list<T>;
		};
		template <class T> struct maybe<false, T>
		{
			using type = type_list<>;
		};

		// Merge two flag typelists: union of convert/aligned, and the max overaligned<N>.
		template <class A, class B> struct merge_flags;
		template <class... A, class... B> struct merge_flags<type_list<A...>, type_list<B...>>
		{
		private:
			using all							= type_list<A..., B...>;
			static constexpr bool has_conv		= contains_convert<all>::value;
			static constexpr bool has_align		= contains_aligned<all>::value;
			static constexpr std::size_t max_ov = max_overaligned<all>::value;
			using conv_list						= typename maybe<has_conv, convert_flag>::type;
			using align_list					= typename maybe<has_align, aligned_flag>::type;
			using over_list						= typename maybe<(max_ov != 0), overaligned_flag<max_ov>>::type;

		public:
			using type = tl_cat_t<tl_cat_t<conv_list, align_list>, over_list>;
		};

		// Convert a typelist of tags back to flags<...>.
		template <class TL> struct list_to_flags;

		template <class... Ts> struct list_to_flags<type_list<Ts...>>
		{
			using type = ::SNAP_NAMESPACE::simd::flags<Ts...>;
		};

	} // namespace detail

	template <class... Flags> struct flags
	{
		static_assert(detail::all_flags<Flags...>::value, "flags<...> parameters must be convert_flag, aligned_flag, or overaligned_flag<N>");

		template <class... Other, class = std::enable_if_t<detail::all_flags<Other...>::value>> friend constexpr auto operator|(flags, flags<Other...>)
		{
			using merged_list = typename detail::merge_flags<detail::type_list<Flags...>, detail::type_list<Other...>>::type;
			using result	  = typename detail::list_to_flags<merged_list>::type;
			return result{};
		}
	};

	inline constexpr flags<> flag_default{};
	inline constexpr flags<convert_flag> flag_convert{};
	inline constexpr flags<aligned_flag> flag_aligned{};
	template <std::size_t N, std::enable_if_t<has_single_bit(static_cast<std::uint64_t>(N)), int> = 0>
	inline constexpr flags<overaligned_flag<N>> flag_overaligned{};

} // namespace simd
SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_SIMD_SIMD_HPP
