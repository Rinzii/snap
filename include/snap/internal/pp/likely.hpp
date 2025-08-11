#pragma once

#include "tiny/predef/expects_bool_condition.hpp"

/// \file likely.hpp
/// \brief Provides the RIOT_LIKELY and RIOT_LIKELY_ATTR macros for branch prediction hints.
/// \ingroup riot_traits

/// \def RIOT_LIKELY(x)
/// \brief Macro to indicate that a boolean expression is likely true, for branch prediction.
/// \note Uses riot::internal::expects_bool_condition to implement the likely hint portably in code.

#ifndef RIOT_LIKELY
	#define RIOT_LIKELY(x) riot::internal::expects_bool_condition((x), true)
#endif // RIOT_LIKELY

/// \def RIOT_LIKELY_ATTR
/// \brief Attribute to hint that a branch is likely to be taken, where supported.
/// \note Expands to [[likely]] if the compiler supports it, or nothing otherwise.

#if defined(__has_cpp_attribute)
#  if __has_cpp_attribute(likely)
#    define RIOT_LIKELY_ATTR [[likely]]
#  else
#    define RIOT_LIKELY_ATTR
#  endif
#else
#  define RIOT_LIKELY_ATTR
#endif
