#pragma once


#if defined(__cpp_conditional_explicit) && __cpp_conditional_explicit >= 201806L
#define SNAP_EXPLICIT_EXPR(...) explicit(__VA_ARGS__)
#else
#define SNAP_EXPLICIT_EXPR(...) explicit
#endif

#if defined(__cpp_conditional_explicit) && __cpp_conditional_explicit >= 201806L
#define SNAP_EXPLICIT_EXPR_ONLY(...) explicit(__VA_ARGS__)
#else
#define SNAP_EXPLICIT_EXPR_ONLY(...)
#endif
