#ifndef SNP_INCLUDE_SNAP_SPAN_HPP
#define SNP_INCLUDE_SNAP_SPAN_HPP

/**
 * @file
 * @brief `snap::span` - a lightweight, non-owning view over contiguous buffers.
 */

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/remove_cvref.hpp"

#include <array>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef SNAP_SPAN_HAS_EXCEPTIONS
	#define SNAP_SPAN_INTERNAL_DEFINED_HAS_EXCEPTIONS 1
	#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
		#define SNAP_SPAN_HAS_EXCEPTIONS 1
	#else
		#define SNAP_SPAN_HAS_EXCEPTIONS 0
	#endif
#endif

#ifndef SNAP_SPAN_CONTRACT_NONE
	#define SNAP_SPAN_CONTRACT_NONE 0
#endif

#ifndef SNAP_SPAN_CONTRACT_TERMINATE
	#define SNAP_SPAN_CONTRACT_TERMINATE 1
#endif

#ifndef SNAP_SPAN_CONTRACT_THROW
	#define SNAP_SPAN_CONTRACT_THROW 2
#endif

#ifdef SNAP_SPAN_DISABLE_CONTRACTS
	#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_NONE
#elif defined(SNAP_SPAN_CONTRACTS_THROW)
	#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_THROW
#elif defined(SNAP_SPAN_CONTRACTS_TERMINATE)
	#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_TERMINATE
#elif !defined(SNAP_SPAN_CONTRACT_MODE)
	#ifdef NDEBUG
		#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_NONE
	#else
		#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_TERMINATE
	#endif
#endif

#if (SNAP_SPAN_CONTRACT_MODE == SNAP_SPAN_CONTRACT_THROW) && !SNAP_SPAN_HAS_EXCEPTIONS
	#undef SNAP_SPAN_CONTRACT_MODE
	#define SNAP_SPAN_CONTRACT_MODE SNAP_SPAN_CONTRACT_TERMINATE
#endif

SNAP_BEGIN_NAMESPACE

/// Sentinel extent value used when the span size is only known at runtime.
inline constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

/// Byte type used by the byte-casting helpers.
using byte = std::byte;

template <typename ElementType, std::size_t Extent = dynamic_extent> class span;

template <typename> struct is_span : std::false_type
{
};

template <typename T, std::size_t S> struct is_span<span<T, S>> : std::true_type
{
};

namespace internal_span_detail
{
#if SNAP_SPAN_CONTRACT_MODE == SNAP_SPAN_CONTRACT_THROW
	struct contract_violation_error : std::logic_error
	{
		explicit contract_violation_error(const char* msg) : std::logic_error(msg) {}
	};

	[[noreturn]] inline void contract_violation(const char* msg)
	{
		throw contract_violation_error(msg);
	}
#elif SNAP_SPAN_CONTRACT_MODE == SNAP_SPAN_CONTRACT_TERMINATE
	[[noreturn]] inline void contract_violation(const char* /*unused*/)
	{
		std::terminate();
	}
#endif

	namespace detail
	{
		template <typename...> using void_t = void;

		template <class C> constexpr auto size(const C& c) -> decltype(c.size())
		{
			return c.size();
		}

		template <class T, std::size_t N> constexpr std::size_t size(const std::array<T, N>& arr) noexcept
		{
			return arr.size();
		}

		template <class T, std::size_t N> constexpr std::size_t size(const T (&array)[N]) noexcept // NOLINT(cppcoreguidelines-avoid-c-arrays)
		{
			return N;
		}

		template <class C> constexpr auto data(C& c) -> decltype(c.data())
		{
			return c.data();
		}

		template <class C> constexpr auto data(const C& c) -> decltype(c.data())
		{
			return c.data();
		}

		template <class T, std::size_t N> constexpr T* data(std::array<T, N>& arr) noexcept
		{
			return arr.data();
		}

		template <class T, std::size_t N> constexpr const T* data(const std::array<T, N>& arr) noexcept
		{
			return arr.data();
		}

		template <class T, std::size_t N> constexpr T* data(T (&array)[N]) noexcept // NOLINT(cppcoreguidelines-avoid-c-arrays)
		{
			return array;
		}

		template <class E> constexpr const E* data(std::initializer_list<E> il) noexcept
		{
			return il.begin();
		}

		template <typename> struct is_std_array : std::false_type
		{
		};

		template <typename T, std::size_t N> struct is_std_array<std::array<T, N>> : std::true_type
		{
		};

		template <typename, typename = void> struct has_size_and_data : std::false_type
		{
		};

		template <typename T> struct has_size_and_data<T, void_t<decltype(detail::size(std::declval<T>())), decltype(detail::data(std::declval<T>()))>>
			: std::true_type
		{
		};

		template <typename C, typename U = remove_cvref_t<C>> struct is_container
		{
			static constexpr bool value = !::SNAP_NAMESPACE::is_span<U>::value && !is_std_array<U>::value && !std::is_array_v<U> && has_size_and_data<C>::value;
		};

		template <typename, typename, typename = void> struct is_container_element_type_compatible : std::false_type
		{
		};

		template <typename T, typename E> struct is_container_element_type_compatible<
			T,
			E,
			std::enable_if_t<!std::is_same_v<std::remove_cv_t<decltype(detail::data(std::declval<T>()))>, void> &&
							 std::is_convertible_v<std::remove_pointer_t<decltype(detail::data(std::declval<T>()))> (*)[],
												   E (*)[]>> // NOLINT(cppcoreguidelines-avoid-c-arrays)
			>												 // NOLINT(cppcoreguidelines-avoid-c-arrays)
			: std::true_type
		{
		};

		template <typename, typename = std::size_t> struct is_complete : std::false_type
		{
		};

		template <typename T> struct is_complete<T, decltype(sizeof(T))> : std::true_type
		{
		};

		template <typename E, std::size_t S> struct span_storage
		{
			constexpr span_storage() noexcept = default;
			constexpr span_storage(E* p_ptr, std::size_t /*unused_size*/) noexcept : ptr(p_ptr) {}
			E* ptr							  = nullptr;
			static constexpr std::size_t size = S;
		};

		template <typename E> struct span_storage<E, static_cast<std::size_t>(-1)>
		{
			constexpr span_storage() noexcept = default;
			constexpr span_storage(E* p_ptr, std::size_t p_size) noexcept : ptr(p_ptr), size(p_size) {}
			E* ptr			 = nullptr;
			std::size_t size = 0;
		};
	} // namespace detail
} // namespace internal_span_detail

#if SNAP_SPAN_CONTRACT_MODE != SNAP_SPAN_CONTRACT_NONE
	#ifndef SNAP_SPAN_STRINGIFY_IMPL
		#define SNAP_SPAN_STRINGIFY_IMPL(x)				  #x
		#define SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY_IMPL 1
	#endif
	#ifndef SNAP_SPAN_STRINGIFY
		#define SNAP_SPAN_STRINGIFY(x)				 SNAP_SPAN_STRINGIFY_IMPL(x)
		#define SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY 1
	#endif
	#ifndef SNAP_SPAN_EXPECT
		#define SNAP_SPAN_EXPECT(cond)                                                                                                                         \
			((cond) ? static_cast<void>(0) : ::SNAP_NAMESPACE::internal_span_detail::contract_violation("Expected " SNAP_SPAN_STRINGIFY(cond)))
		#define SNAP_SPAN_INTERNAL_DEFINED_EXPECT 1
	#endif
#else
	#ifndef SNAP_SPAN_EXPECT
		#define SNAP_SPAN_EXPECT(cond)			  static_cast<void>(0)
		#define SNAP_SPAN_INTERNAL_DEFINED_EXPECT 1
	#endif
#endif

/**
 * @brief Non-owning view over a contiguous sequence of objects.
 *
 * @tparam ElementType element type referenced by the view.
 * @tparam Extent number of elements known at compile time or `dynamic_extent`.
 */
template <typename ElementType, std::size_t Extent> class span
{
	static_assert(std::is_object_v<ElementType>);
	static_assert(internal_span_detail::detail::is_complete<ElementType>::value);
	static_assert(!std::is_abstract_v<ElementType>);

	using storage_type = internal_span_detail::detail::span_storage<ElementType, Extent>;

public:
	using element_type			 = ElementType;
	using value_type			 = std::remove_cv_t<ElementType>;
	using size_type				 = std::size_t;
	using difference_type		 = std::ptrdiff_t;
	using pointer				 = element_type*;
	using const_pointer			 = const element_type*;
	using reference				 = element_type&;
	using const_reference		 = const element_type&;
	using iterator				 = pointer;
	using const_iterator		 = const_pointer;
	using reverse_iterator		 = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	static constexpr size_type extent = Extent;

	/// Constructs an empty span when the extent is `dynamic_extent` or zero.
	template <std::size_t E = Extent, std::enable_if_t<(E == dynamic_extent || E == 0), int> = 0> constexpr span() noexcept : storage_(nullptr, size_type{}) {}

	/// Constructs a span over `[ptr, ptr + count)`.
	constexpr span(pointer ptr, size_type count) : storage_(ptr, count) { SNAP_SPAN_EXPECT(extent == dynamic_extent || count == extent); }

	/// Constructs a span over `[first_elem, last_elem)`.
	constexpr span(pointer first_elem, pointer last_elem) : storage_(first_elem, static_cast<size_type>(last_elem - first_elem))
	{
		SNAP_SPAN_EXPECT(extent == dynamic_extent || (last_elem - first_elem) == static_cast<std::ptrdiff_t>(extent));
	}

	template <std::size_t N,
			  std::size_t E			= Extent,
			  std::enable_if_t<(E == dynamic_extent || N == E) &&
								   internal_span_detail::detail::is_container_element_type_compatible<element_type (&)[N], ElementType>::
									   value, // NOLINT(cppcoreguidelines-avoid-c-arrays)
							   int> = 0>
	constexpr span(element_type (&arr)[N]) noexcept // NOLINT(google-explicit-constructor) NOLINT(cppcoreguidelines-avoid-c-arrays)
		: storage_(arr, N)							// NOLINT(cppcoreguidelines-avoid-c-arrays)
	{
	}

	template <typename T,
			  std::size_t N,
			  std::size_t E			= Extent,
			  std::enable_if_t<(E == dynamic_extent || N == E) &&
								   internal_span_detail::detail::is_container_element_type_compatible<std::array<T, N>&, ElementType>::value,
							   int> = 0>
	constexpr span(std::array<T, N>& arr) noexcept // NOLINT(google-explicit-constructor)
		: storage_(arr.data(), N)
	{
	}

	template <typename T,
			  std::size_t N,
			  std::size_t E			= Extent,
			  std::enable_if_t<(E == dynamic_extent || N == E) &&
								   internal_span_detail::detail::is_container_element_type_compatible<const std::array<T, N>&, ElementType>::value,
							   int> = 0>
	constexpr span(const std::array<T, N>& arr) noexcept // NOLINT(google-explicit-constructor)
		: storage_(arr.data(), N)
	{
	}

	template <typename Container,
			  std::size_t E			= Extent,
			  std::enable_if_t<E == dynamic_extent && internal_span_detail::detail::is_container<Container>::value &&
								   internal_span_detail::detail::is_container_element_type_compatible<Container&, ElementType>::value,
							   int> = 0>
	constexpr span(Container& cont) // NOLINT(google-explicit-constructor)
		: storage_(internal_span_detail::detail::data(cont), internal_span_detail::detail::size(cont))
	{
	}

	template <typename Container,
			  std::size_t E			= Extent,
			  std::enable_if_t<E == dynamic_extent && internal_span_detail::detail::is_container<Container>::value &&
								   internal_span_detail::detail::is_container_element_type_compatible<const Container&, ElementType>::value,
							   int> = 0>
	constexpr span(const Container& cont) // NOLINT(google-explicit-constructor)
		: storage_(internal_span_detail::detail::data(cont), internal_span_detail::detail::size(cont))
	{
	}

	constexpr span(const span&) noexcept = default;
	constexpr span(span&&) noexcept		 = default;
	~span() noexcept					 = default;

	template <typename OtherElementType,
			  std::size_t OtherExtent,
			  std::enable_if_t<(Extent == dynamic_extent || OtherExtent == dynamic_extent || Extent == OtherExtent) &&
								   std::is_convertible_v<OtherElementType (*)[], ElementType (*)[]>, // NOLINT(cppcoreguidelines-avoid-c-arrays)
							   int> = 0>
	constexpr span(const span<OtherElementType, OtherExtent>& other) noexcept // NOLINT(google-explicit-constructor)
		: storage_(other.data(), other.size())
	{
	}

	span& operator=(const span&) noexcept = default;
	span& operator=(span&&) noexcept	  = default;

	/// Returns a span containing the first `Count` elements.
	template <std::size_t Count> constexpr span<element_type, Count> first() const
	{
		SNAP_SPAN_EXPECT(Count <= size());
		return { data(), Count };
	}

	/// Returns a span containing the last `Count` elements.
	template <std::size_t Count> constexpr span<element_type, Count> last() const
	{
		SNAP_SPAN_EXPECT(Count <= size());
		return { data() + (size() - Count), Count };
	}

	template <std::size_t Offset, std::size_t Count = dynamic_extent> struct subspan_extent
	{
	private:
		static constexpr size_type compute() noexcept
		{
			if constexpr (Count != dynamic_extent) { return Count; }
			else if constexpr (Extent != dynamic_extent) { return Extent - Offset; }
			else
			{
				return dynamic_extent;
			}
		}

	public:
		static constexpr size_type value = compute();
	};

	template <std::size_t Offset, std::size_t Count = dynamic_extent> using subspan_return_t = span<ElementType, subspan_extent<Offset, Count>::value>;

	/// Returns a span describing the subrange at `Offset` for `Count` elements (if provided).
	template <std::size_t Offset, std::size_t Count = dynamic_extent> constexpr subspan_return_t<Offset, Count> subspan() const
	{
		SNAP_SPAN_EXPECT(Offset <= size() && (Count == dynamic_extent || Offset + Count <= size()));
		return { data() + Offset, Count != dynamic_extent ? Count : size() - Offset };
	}

	/// Returns a span containing the first `count` elements.
	constexpr span<element_type, dynamic_extent> first(size_type count) const
	{
		SNAP_SPAN_EXPECT(count <= size());
		return { data(), count };
	}

	/// Returns a span containing the last `count` elements.
	constexpr span<element_type, dynamic_extent> last(size_type count) const
	{
		SNAP_SPAN_EXPECT(count <= size());
		return { data() + (size() - count), count };
	}

	/// Returns a span describing the subrange starting at `offset`.
	constexpr span<element_type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const
	{
		SNAP_SPAN_EXPECT(offset <= size() && (count == dynamic_extent || offset + count <= size()));
		return { data() + offset, count == dynamic_extent ? size() - offset : count };
	}

	[[nodiscard]] constexpr size_type size() const noexcept { return storage_.size; }

	[[nodiscard]] constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }

	[[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

	constexpr reference operator[](size_type idx) const
	{
		SNAP_SPAN_EXPECT(idx < size());
		return *(data() + idx);
	}

	constexpr reference front() const
	{
		SNAP_SPAN_EXPECT(!empty());
		return *data();
	}

	constexpr reference back() const
	{
		SNAP_SPAN_EXPECT(!empty());
		return *(data() + (size() - 1));
	}

	constexpr pointer data() const noexcept { return storage_.ptr; }

	constexpr iterator begin() const noexcept { return data(); }

	constexpr iterator end() const noexcept { return data() + size(); }

	constexpr const_iterator cbegin() const noexcept { return data(); }

	constexpr const_iterator cend() const noexcept { return data() + size(); }

	constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

	constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

	constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

	constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

private:
	storage_type storage_{};
};

template <class T, std::size_t N> span(T (&)[N]) -> span<T, N>; // NOLINT(cppcoreguidelines-avoid-c-arrays)

template <class T, std::size_t N> span(std::array<T, N>&) -> span<T, N>;

template <class T, std::size_t N> span(const std::array<T, N>&) -> span<const T, N>;

template <class Container> span(Container&)
	-> span<std::remove_reference_t<decltype(*internal_span_detail::detail::data(std::declval<Container&>()))>, dynamic_extent>;

template <class Container> span(const Container&) -> span<const typename Container::value_type, dynamic_extent>;

/// Convenience wrapper that preserves an existing span.
template <typename ElementType, std::size_t Extent> constexpr span<ElementType, Extent> make_span(span<ElementType, Extent> s) noexcept
{
	return s;
}

/// Creates a span from a C array.
template <typename T, std::size_t N> constexpr span<T, N> make_span(T (&arr)[N]) noexcept // NOLINT(cppcoreguidelines-avoid-c-arrays)
{
	return { arr };
}

/// Creates a span from a mutable std::array reference.
template <typename T, std::size_t N> constexpr span<T, N> make_span(std::array<T, N>& arr) noexcept
{
	return { arr };
}

/// Creates a span from a const std::array reference.
template <typename T, std::size_t N> constexpr span<const T, N> make_span(const std::array<T, N>& arr) noexcept
{
	return { arr };
}

/// Creates a span from an arbitrary container exposing contiguous storage.
template <typename Container>
constexpr span<std::remove_reference_t<decltype(*internal_span_detail::detail::data(std::declval<Container&>()))>, dynamic_extent> make_span(Container& cont)
{
	return { cont };
}

/// Creates a const span from an arbitrary container exposing contiguous storage.
template <typename Container> constexpr span<const typename Container::value_type, dynamic_extent> make_span(const Container& cont)
{
	return { cont };
}

/// Produces a `span<const byte>` view over the referenced elements.
template <typename ElementType, std::size_t Extent>
span<const byte, (Extent == dynamic_extent ? dynamic_extent : (sizeof(ElementType) * Extent))> as_bytes(span<ElementType, Extent> s) noexcept
{
	return { reinterpret_cast<const byte*>(s.data()), s.size_bytes() }; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

/// Produces a writable `span<byte>` view over the referenced elements.
template <class ElementType, std::size_t Extent, std::enable_if_t<!std::is_const_v<ElementType>, int> = 0>
span<byte, (Extent == dynamic_extent ? dynamic_extent : (sizeof(ElementType) * Extent))> as_writable_bytes(span<ElementType, Extent> s) noexcept
{
	return { reinterpret_cast<byte*>(s.data()), s.size_bytes() }; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

/// Tuple-like accessor enabling structured bindings for `span`.
template <std::size_t N, typename E, std::size_t S> constexpr decltype(auto) get(span<E, S> s)
{
	return s[N];
}

SNAP_END_NAMESPACE

namespace std
{
	template <typename ElementType, std::size_t Extent> class tuple_size<::SNAP_NAMESPACE::span<ElementType, Extent>>
		: public integral_constant<std::size_t, Extent>
	{
	};

	template <typename ElementType> class tuple_size<::SNAP_NAMESPACE::span<ElementType, ::SNAP_NAMESPACE::dynamic_extent>>;

	template <std::size_t I, typename ElementType, std::size_t Extent> class tuple_element<I, ::SNAP_NAMESPACE::span<ElementType, Extent>>
	{
	public:
		static_assert(Extent != ::SNAP_NAMESPACE::dynamic_extent && I < Extent, "span tuple_element index out of range");
		using type = ElementType;
	};
} // namespace std

#ifdef SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY
	#undef SNAP_SPAN_STRINGIFY
	#undef SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY
#endif

#ifdef SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY_IMPL
	#undef SNAP_SPAN_STRINGIFY_IMPL
	#undef SNAP_SPAN_INTERNAL_DEFINED_STRINGIFY_IMPL
#endif

#ifdef SNAP_SPAN_INTERNAL_DEFINED_EXPECT
	#undef SNAP_SPAN_EXPECT
	#undef SNAP_SPAN_INTERNAL_DEFINED_EXPECT
#endif

#ifdef SNAP_SPAN_INTERNAL_DEFINED_HAS_EXCEPTIONS
	#undef SNAP_SPAN_HAS_EXCEPTIONS
	#undef SNAP_SPAN_INTERNAL_DEFINED_HAS_EXCEPTIONS
#endif

#endif // SNP_INCLUDE_SNAP_SPAN_HPP
