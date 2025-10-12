#pragma once

#include "tiny/predef/expects_bool_condition.hpp"

/// \file likely.hpp
/// \brief Provides the SNAP_LIKELY and SNAP_LIKELY_ATTR macros for branch prediction hints.
/// \ingroup SNAP_traits

/// \def SNAP_LIKELY(x)
/// \brief Macro to indicate that a boolean expression is likely true, for branch prediction.
/// \note Uses snap::internal::expects_bool_condition to implement the likely hint portably in code.

#ifndef SNAP_LIKELY
	#define SNAP_LIKELY(x) snap::internal::expects_bool_condition((x), true)
#endif // SNAP_LIKELY

/// \def SNAP_LIKELY_ATTR
/// \brief Attribute to hint that a branch is likely to be taken, where supported.
/// \note Expands to [[likely]] if the compiler supports it, or nothing otherwise.

#if defined(__has_cpp_attribute)
	#if __has_cpp_attribute(likely)
		#define SNAP_LIKELY_ATTR [[likely]]
	#else
		#define SNAP_LIKELY_ATTR
	#endif
#else
	#define SNAP_LIKELY_ATTR
#endif
