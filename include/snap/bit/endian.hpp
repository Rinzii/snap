/*
Supported platforms detected by this header

OS / libc families
- Windows (MSVC, clang-cl) via _WIN32
- Linux (glibc, musl) via <endian.h> and arch macros
- macOS, iOS, tvOS, watchOS via <machine/endian.h> and arch macros
- FreeBSD, NetBSD, OpenBSD, DragonFly via <sys/endian.h>, <sys/param.h>, <machine/endian.h>
- Solaris/Illumos via <sys/isa_defs.h> and arch macros
- AIX via <gulliver.h> and POWER macros
- Emscripten/WebAssembly via __EMSCRIPTEN__ / __wasm__ (Wasm is always little-endian)

Architectures (little-endian path)
- x86, x86_64
- ARM/Thumb LE, AArch64 LE
- RISC-V (LE assumed)
- PowerPC64 LE (ppc64le)
- MIPS LE
- LoongArch
- Nios II LE
- SuperH LE
- Alpha, AVR, Blackfin, LM32, MSP430
- Renesas RX
- OpenRISC LE (when macro indicates LE)
- MicroBlaze EL
- ARC LE (when macro indicates LE)

Architectures (big-endian path)
- ARM/Thumb BE, AArch64 BE
- MIPS BE
- PowerPC 32/64 BE
- IBM System z (s390, s390x)
- SPARC (32/64)
- m68k, PA-RISC, H8/300
- Nios II BE
- SuperH BE
- OpenRISC BE (when macro indicates BE)
- MicroBlaze EB (or MicroBlaze without EL)
- ARC BE (when macro indicates BE)

Other notes
- TILE-Gx, TILE-Pro, Xtensa, Itanium: endianness is toolchain/OS-selected; do not assume from arch macro alone.

Mixed-endian
- Supported: endian::native may differ from endian::little and endian::big on platforms that report mixed (e.g., PDP 3412).

Configuration
- Define one of SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN to override.
- Define SNAP_DISABLE_ENDIAN_ARCH_HINTS to skip architecture hints.
*/

#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#if defined(__has_include)
	#if !defined(__BYTE_ORDER__) && __has_include(<endian.h>)
		#include <endian.h>
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<machine/endian.h>)
		#include <machine/endian.h>
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/param.h>)
		#include <sys/param.h>
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/endian.h>)
		#include <sys/endian.h>
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/isa_defs.h>)
		#include <sys/isa_defs.h>
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<gulliver.h>)
		#include <gulliver.h>
	#endif
#endif

#if defined(SNAP_ASSUME_LITTLE_ENDIAN) && defined(SNAP_ASSUME_BIG_ENDIAN)
	#error "Define only one of SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

SNAP_BEGIN_NAMESPACE
namespace detail
{

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

#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN__)
	constexpr bool native_is_little = true;

#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(_BIG_ENDIAN__)
	constexpr bool native_is_little = false;

#elif defined(__LITTLEENDIAN__) && !defined(__BIGENDIAN__)
	constexpr bool native_is_little = true;

#elif defined(__BIGENDIAN__) && !defined(__LITTLEENDIAN__)
	constexpr bool native_is_little = false;

#elif !defined(SNAP_DISABLE_ENDIAN_ARCH_HINTS) &&                                                                                                              \
	(defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64) || defined(__alpha__) || defined(__AVR__) || defined(__bfin__) ||         \
	 defined(__nios2_little_endian__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) ||                                               \
	 (defined(__arm__) && defined(__BYTE_ORDER_LITTLE_ENDIAN__)) || (defined(__aarch64__) && !defined(__AARCH64EB__)) || defined(__MIPSEL__) ||                \
	 (defined(__mips__) && defined(__MIPSEL)) || defined(__loongarch__) || (defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)) || defined(__ppc64le__) ||   \
	 ((defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)) && defined(__LITTLE_ENDIAN__)) || (defined(__sh__) && defined(__LITTLE_ENDIAN__)) ||      \
	 defined(__MSP430__) || defined(__RX__) || (defined(__or1k__) && defined(__LITTLE_ENDIAN__)) || defined(__MICROBLAZEEL__) ||                               \
	 (defined(__arc__) && (defined(__LITTLE_ENDIAN__) || defined(__ARC_LITTLE_ENDIAN__))))
	constexpr bool native_is_little = true;

#elif !defined(SNAP_DISABLE_ENDIAN_ARCH_HINTS) &&                                                                                                              \
	(defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || (defined(__arm__) && defined(__BYTE_ORDER_BIG_ENDIAN__)) ||                       \
	 (defined(__mips__) && (defined(__MIPSEB) || defined(__MIPSEB__))) || defined(__MIPSEB__) ||                                                               \
	 ((defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)) && !defined(__LITTLE_ENDIAN__)) || (defined(__PPC64__) && !defined(__LITTLE_ENDIAN__)) || \
	 defined(__s390__) || defined(__s390x__) || defined(__sparc) || defined(__sparc__) || defined(__sparc64__) || defined(__m68k__) || defined(__hppa__) ||    \
	 defined(__nios2_big_endian__) || (defined(__sh__) && defined(__BIG_ENDIAN__)) || (defined(__or1k__) && defined(__BIG_ENDIAN__)) ||                        \
	 defined(__MICROBLAZEEB__) || (defined(__MICROBLAZE__) && !defined(__MICROBLAZEEL__)) ||                                                                   \
	 (defined(__arc__) && (defined(__BIG_ENDIAN__) || defined(__ARC_BIG_ENDIAN__))) || defined(__H8300__) || defined(__h8300__))
	constexpr bool native_is_little = false;

#else
	#error "SNAP_NAMESPACE::endian: unable to determine endianness. Define SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
	constexpr int little_value = __ORDER_LITTLE_ENDIAN__;
	constexpr int big_value	   = __ORDER_BIG_ENDIAN__;
#else
	constexpr int little_value = 1234;
	constexpr int big_value	   = 4321;
#endif

#if defined(__BYTE_ORDER__)
	constexpr int native_value = __BYTE_ORDER__;
#elif defined(BYTE_ORDER)
	constexpr int native_value = BYTE_ORDER;
#else
	constexpr int native_value = native_is_little ? little_value : big_value;
#endif

} // namespace detail

enum class endian : int
{
	little = detail::little_value,
	big	   = detail::big_value,
	native = detail::native_value
};
SNAP_END_NAMESPACE
