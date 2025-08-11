#pragma once

#if defined(__has_include)
#  if !defined(__BYTE_ORDER__) && __has_include(<endian.h>)
#    include <endian.h>           // glibc, musl
#  endif
#  if !defined(__BYTE_ORDER__) && __has_include(<machine/endian.h>)
#    include <machine/endian.h>   // *BSD, macOS
#  endif
#  if !defined(__BYTE_ORDER__) && __has_include(<sys/param.h>)
#    include <sys/param.h>        // some BSDs
#  endif
#  if !defined(__BYTE_ORDER__) && __has_include(<sys/endian.h>)
#    include <sys/endian.h>       // *BSD variant
#  endif
#endif

// Optional user overrides (define before including this header):
//   #define SNAP_ASSUME_LITTLE_ENDIAN 1
//   #define SNAP_ASSUME_BIG_ENDIAN    1
#if defined(SNAP_ASSUME_LITTLE_ENDIAN) && defined(SNAP_ASSUME_BIG_ENDIAN)
#  error "Define only one of SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

namespace snap {

namespace detail {

// Compile-time probe for native endianness (no <bit>, no std::endian).
#if defined(SNAP_ASSUME_LITTLE_ENDIAN)
constexpr bool native_is_little = true;
#elif defined(SNAP_ASSUME_BIG_ENDIAN)
constexpr bool native_is_little = false;
#elif defined(_WIN32) || defined(__EMSCRIPTEN__) || defined(__wasm__)
constexpr bool native_is_little = true;
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
constexpr bool native_is_little = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
#elif defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
constexpr bool native_is_little = (BYTE_ORDER == LITTLE_ENDIAN);
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
constexpr bool native_is_little = true;
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
constexpr bool native_is_little = false;
// Architecture hints:
#elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__)
constexpr bool native_is_little = true;
#elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__)
constexpr bool native_is_little = false;
#elif defined(__MIPSEL__)
constexpr bool native_is_little = true;
#elif defined(__MIPSEB__)
constexpr bool native_is_little = false;
#elif defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
constexpr bool native_is_little = true;
#elif defined(__riscv)
constexpr bool native_is_little = true;
#elif defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)
constexpr bool native_is_little = true;
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
constexpr bool native_is_little = false;
#elif defined(__s390__) || defined(__s390x__)
constexpr bool native_is_little = false;
#elif defined(__sparc) || defined(__sparc__)
constexpr bool native_is_little = false;
#elif defined(__loongarch__)
constexpr bool native_is_little = true;
#elif defined(__m68k__)
constexpr bool native_is_little = false;
#else
#  error "snap::endian: unable to determine endianness. Define SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

// Enumerator values: prefer compiler order macros; else fall back to 0/1.
#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
constexpr int little_value = __ORDER_LITTLE_ENDIAN__;
constexpr int big_value    = __ORDER_BIG_ENDIAN__;
#else
constexpr int little_value = 0;
constexpr int big_value    = 1;
#endif

} // namespace detail

enum class endian : int {
    little = detail::little_value,
    big    = detail::big_value,
    native = detail::native_is_little ? little : big
};

#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
static_assert(static_cast<int>(endian::little) == __ORDER_LITTLE_ENDIAN__, "endian::little mismatch");
static_assert(static_cast<int>(endian::big)    == __ORDER_BIG_ENDIAN__,    "endian::big mismatch");
#endif
static_assert(static_cast<int>(endian::little) != static_cast<int>(endian::big), "little/big must differ");

} // namespace snap
