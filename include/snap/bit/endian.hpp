#pragma once

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
	- Alpha, AVR, Blackfin, Xtensa*, LM32, MSP430
	- Renesas RX
	- OpenRISC LE (when macro indicates LE)
	- MicroBlaze LE
	- ARC LE

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
	- MicroBlaze BE
	- ARC BE

Other notes
	- TILE-Gx, TILE-Pro: endianness is toolchain-selected; do not assume from the arch macro.
	- Itanium (ia64): endianness depends on OS/toolchain; do not assume.
	- Xtensa*: can be built LE or BE via compiler flags; endianness must come from platform byte-order macros.

Not supported
	- PDP/mixed-endian: hard error if detected

Configuration notes
	- You can force detection with SNAP_ASSUME_LITTLE_ENDIAN=1 or SNAP_ASSUME_BIG_ENDIAN=1
	- You can disable architecture hints with SNAP_DISABLE_ENDIAN_ARCH_HINTS=1
	- RISC-V BE is not auto-detected; define SNAP_ASSUME_BIG_ENDIAN=1 (or add a project macro) if needed
*/

#if defined(__has_include)
	#if !defined(__BYTE_ORDER__) && __has_include(<endian.h>)
		#include <endian.h> // glibc, musl
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<machine/endian.h>)
		#include <machine/endian.h> // *BSD, macOS
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/param.h>)
		#include <sys/param.h> // some BSDs
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/endian.h>)
		#include <sys/endian.h> // *BSD variant
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<sys/isa_defs.h>)
		#include <sys/isa_defs.h> // Solaris
	#endif
	#if !defined(__BYTE_ORDER__) && __has_include(<gulliver.h>)
		#include <gulliver.h> // IBM AIX
	#endif
#endif

// Enforce the user only selected one option
#if defined(SNAP_ASSUME_LITTLE_ENDIAN) && defined(SNAP_ASSUME_BIG_ENDIAN)
	#error "Define only one of SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

// PDP-endian is not supported as it is mixed endian and rare
#if defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
	#error "PDP-endian is unsupported."
#endif

namespace snap
{
	namespace detail
	{
#if defined(SNAP_ASSUME_LITTLE_ENDIAN)
		constexpr bool native_is_little = true;

#elif defined(SNAP_ASSUME_BIG_ENDIAN)
		constexpr bool native_is_little = false;

#elif defined(_WIN32) || defined(__EMSCRIPTEN__) || defined(__wasm__)
		constexpr bool native_is_little = true; // Windows/Web

#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
		constexpr bool native_is_little = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__); // GCC/Clang

#elif defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
		constexpr bool native_is_little = (BYTE_ORDER == LITTLE_ENDIAN); // BSD-style

#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN__)
		constexpr bool native_is_little = true; // Generic LE macro

#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(_BIG_ENDIAN__)
		constexpr bool native_is_little = false; // Generic BE macro

#elif defined(__LITTLEENDIAN__) && !defined(__BIGENDIAN__)
		constexpr bool native_is_little = true; // Alt macros

#elif defined(__BIGENDIAN__) && !defined(__LITTLEENDIAN__)
		constexpr bool native_is_little = false;

#elif !defined(SNAP_DISABLE_ENDIAN_ARCH_HINTS) &&                                                                                                              \
	(defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64) || defined(__riscv) || defined(__alpha__) || defined(__AVR__) ||          \
	 defined(__bfin__) || defined(__xtensa__) || defined(__lm32__) || defined(__nios2_little_endian__) || defined(__ARMEL__) || defined(__THUMBEL__) ||        \
	 defined(__AARCH64EL__) || (defined(__arm__) && defined(__BYTE_ORDER_LITTLE_ENDIAN__)) || (defined(__aarch64__) && !defined(__AARCH64EB__)) ||             \
	 defined(__MIPSEL__) || (defined(__mips__) && defined(__MIPSEL)) || defined(__loongarch__) || (defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)) ||    \
	 defined(__ppc64le__) || ((defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)) && defined(__LITTLE_ENDIAN__)) ||                                 \
	 (defined(__sh__) && defined(__LITTLE_ENDIAN__)) || defined(__h8300__) || defined(__MSP430__) || defined(__ia64__) || defined(__RX__) ||                   \
	 (defined(__or1k__) && defined(__LITTLE_ENDIAN__)) || defined(__MICROBLAZEEL__) ||                                                                         \
	 (defined(__arc__) && (defined(__LITTLE_ENDIAN__) || defined(__ARC_LITTLE_ENDIAN__))))
		constexpr bool native_is_little = true;

#elif !defined(SNAP_DISABLE_ENDIAN_ARCH_HINTS) &&                                                                                                              \
	(defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || (defined(__arm__) && defined(__BYTE_ORDER_BIG_ENDIAN__)) ||                       \
	 (defined(__mips__) && (defined(__MIPSEB) || defined(__MIPSEB__))) || defined(__MIPSEB__) ||                                                               \
	 ((defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)) && !defined(__LITTLE_ENDIAN__)) || (defined(__PPC64__) && !defined(__LITTLE_ENDIAN__)) || \
	 defined(__s390__) || defined(__s390x__) || defined(__sparc) || defined(__sparc__) || defined(__sparc64__) || defined(__m68k__) || defined(__hppa__) ||    \
	 defined(__nios2_big_endian__) || (defined(__sh__) && defined(__BIG_ENDIAN__)) || (defined(__or1k__) && defined(__BIG_ENDIAN__)) ||                        \
	 (defined(__MICROBLAZE__) && !defined(__MICROBLAZEEL__)) || (defined(__arc__) && (defined(__BIG_ENDIAN__) || defined(__ARC_BIG_ENDIAN__))))
		constexpr bool native_is_little = false;

#else
	#error "snap::endian: unable to determine endianness. Define SNAP_ASSUME_LITTLE_ENDIAN or SNAP_ASSUME_BIG_ENDIAN."
#endif

#if defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
		constexpr int little_value = __ORDER_LITTLE_ENDIAN__;
		constexpr int big_value	   = __ORDER_BIG_ENDIAN__;
#elif defined(_LITTLE_ENDIAN) && defined(_BIG_ENDIAN)
		constexpr int little_value = 1234; // legacy values
		constexpr int big_value	   = 4321;
#else
		constexpr int little_value = 0;
		constexpr int big_value	   = 1;
#endif

	} // namespace detail

	enum class endian : int
	{
		little = detail::little_value,
		big	   = detail::big_value,
		native = detail::native_is_little ? little : big
	};
} // namespace snap
