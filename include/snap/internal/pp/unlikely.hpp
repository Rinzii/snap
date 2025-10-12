#pragma once

/// \file unlikely.hpp
/// \brief Provides the SNAP_UNLIKELY macro and SNAP_UNLIKELY_ATTR attribute for branch prediction hints.
/// \ingroup SNAP_branch_prediction

#include "tiny/predef/expects_bool_condition.hpp"

/// \def SNAP_UNLIKELY(x)
/// \brief Hints to the compiler that the given boolean expression is unlikely to be true.
/// \param x Boolean condition expected to be false.
/// \note Internally enforces that the parameter is a boolean condition (see expects_bool_condition).
#ifndef SNAP_UNLIKELY
	#define SNAP_UNLIKELY(x) snap::internal::expects_bool_condition((x), false)
#endif // SNAP_UNLIKELY

/// \def SNAP_UNLIKELY_ATTR
/// \brief Attribute for marking a statement or branch as unlikely (C++20 [[likely]] where supported).
/// \note Falls back to nothing on unsupported compilers.
#if defined(__has_cpp_attribute)
	#if __has_cpp_attribute(likely)
		#define SNAP_UNLIKELY_ATTR [[likely]]
	#else
		#define SNAP_UNLIKELY_ATTR
	#endif
#else
	#define SNAP_UNLIKELY_ATTR
#endif
