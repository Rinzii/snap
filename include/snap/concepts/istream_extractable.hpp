// snap/concepts/istream_extractable.hpp
#ifndef SNP_INCLUDE_SNAP_CONCEPTS_ISTREAM_EXTRACTABLE_HPP
#define SNP_INCLUDE_SNAP_CONCEPTS_ISTREAM_EXTRACTABLE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if !defined(__cpp_concepts) || (__cpp_concepts < 201907L)
	#error "snap/concepts/istream_extractable.hpp requires C++20 concepts."
#endif

#ifdef __has_include
	#if !__has_include(<concepts>)
		#error "snap/concepts/istream_extractable.hpp requires the <concepts> header."
	#endif
#endif

#include <concepts>
#include <iosfwd>
#include <istream>

SNAP_BEGIN_NAMESPACE

namespace concepts
{
	template <class T, class CharT = char, class Traits = std::char_traits<CharT>>
	concept istream_extractable = requires(std::basic_istream<CharT, Traits>& is, T& v) {
		{ is >> v } -> std::same_as<std::basic_istream<CharT, Traits>&>;
	};
} // namespace concepts

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_CONCEPTS_ISTREAM_EXTRACTABLE_HPP
