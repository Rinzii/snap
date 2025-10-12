#pragma once

#if defined(_MSC_VER)
	#if defined(_MSVC_LANG)
		#define SNAP_CPLUSPLUS _MSVC_LANG
	#else
		#define SNAP_CPLUSPLUS 0L
	#endif
#else
	#define SNAP_CPLUSPLUS __cplusplus
#endif

// Require C++17 or newer.
#if SNAP_CPLUSPLUS < 201703L
	#error "Requires C++17 or later."
#endif

// Version flags
#define SNAP_HAS_CPP20 (SNAP_CPLUSPLUS >= 202002L)
#define SNAP_HAS_CPP23 (SNAP_CPLUSPLUS >= 202302L)
#define SNAP_HAS_CPP26 (SNAP_CPLUSPLUS >= 202602L) // TODO: Update this when wg14 defines it. Currently best guess

#define SNAP_LANG_AT_LEAST(CXX) (SNAP_CPLUSPLUS >= (CXX##L))
