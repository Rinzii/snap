#pragma once

// TODO: Once C++26's __cplusplus value has been finalized, update this file to use it.

// Normalize active language level without relying on /Zc:__cplusplus.
#if defined(_MSC_VER)
  #if defined(_MSVC_LANG)
    #define SNAP_CPLUSPLUS _MSVC_LANG
  #else
    // Old MSVC that doesn't define _MSVC_LANG is not supported (we require C++17+).
    #define SNAP_CPLUSPLUS 0L
  #endif
#else
  #define SNAP_CPLUSPLUS __cplusplus
#endif

// Require C++17 or newer.
#if SNAP_CPLUSPLUS < 201703L
#  error "Requires C++17 or later."
#endif

// Version flags
#define SNAP_HAS_CPP20 (SNAP_CPLUSPLUS >= 202002L)
#define SNAP_HAS_CPP23 (SNAP_CPLUSPLUS >= 202302L)

// Canonical current standard (17, 20, or 23)
#if SNAP_HAS_CPP23
#  define SNAP_CXX_STD 23
#elif SNAP_HAS_CPP20
#  define SNAP_CXX_STD 20
#else
#  define SNAP_CXX_STD 17
#endif

// constexpr mirrors (for use in constant expressions without macros)
namespace snap::internal::compat {
  inline constexpr long  cplusplus = SNAP_CPLUSPLUS;                 // 201703L, 202002L, 202302L...
  inline constexpr int   cxx_std   = SNAP_CXX_STD;                   // 17, 20, 23
  inline constexpr bool  has_cpp20 = SNAP_HAS_CPP20;
  inline constexpr bool  has_cpp23 = SNAP_HAS_CPP23;
} // namespace snap::internal::compat

