#pragma once

#include "snap/internal/compat/std.hpp"

// TODO: Update this once std.hpp is finalized for C++26.
#if __cplusplus >= 202602L // currently best guess
  #define SNAP_CONSTEXPR26 constexpr
#else
  #define SNAP_CONSTEXPR26
#endif

#if SNAP_HAS_CPP23
  #define SNAP_CONSTEXPR23 constexpr
#else
  #define SNAP_CONSTEXPR23
#endif

#if SNAP_HAS_CPP20
  #define SNAP_CONSTEXPR20 constexpr
#else
  #define SNAP_CONSTEXPR20
#endif

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
  #define SNAP_CONSTEVAL consteval
#else
  #define SNAP_CONSTEVAL constexpr
#endif
