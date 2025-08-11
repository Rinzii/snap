#pragma once

/// \file unlikely.hpp
/// \brief Provides the RIOT_UNLIKELY macro and RIOT_UNLIKELY_ATTR attribute for branch prediction hints.
/// \ingroup riot_branch_prediction

#include "tiny/predef/expects_bool_condition.hpp"

/// \def RIOT_UNLIKELY(x)
/// \brief Hints to the compiler that the given boolean expression is unlikely to be true.
/// \param x Boolean condition expected to be false.
/// \note Internally enforces that the parameter is a boolean condition (see expects_bool_condition).
#ifndef RIOT_UNLIKELY
	#define RIOT_UNLIKELY(x) riot::internal::expects_bool_condition((x), false)
#endif // RIOT_UNLIKELY

/// \def RIOT_UNLIKELY_ATTR
/// \brief Attribute for marking a statement or branch as unlikely (C++20 [[likely]] where supported).
/// \note Falls back to nothing on unsupported compilers.
#if defined(__has_cpp_attribute)
#  if __has_cpp_attribute(likely)
#    define RIOT_UNLIKELY_ATTR [[likely]]
#  else
#    define RIOT_UNLIKELY_ATTR
#  endif
#else
#  define RIOT_UNLIKELY_ATTR
#endif
