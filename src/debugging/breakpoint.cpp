#include "snap/debugging/breakpoint.hpp"

#include <csignal>
#include <cstdlib>

#if defined(_MSC_VER)
	#include <intrin.h>
#endif

#if defined(__EMSCRIPTEN__)
	#include <emscripten/emscripten.h>
#endif

#if defined(__has_builtin)
	#define SNAP_HAS_BUILTIN(x) __has_builtin(x)
#else
	#define SNAP_HAS_BUILTIN(x) 0
#endif

#if defined(_WIN32)
extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long, unsigned long, unsigned long, const void *);
#endif

SNAP_BEGIN_NAMESPACE

void breakpoint() noexcept
{
#if defined(_MSC_VER)
	// ReSharper disable once CppDFAUnreachableCode
	__debugbreak();
	return;
#endif

#if defined(__EMSCRIPTEN__)
	// ReSharper disable once CppDFAUnreachableCode
	emscripten_debugger();
	return;
#endif

#if SNAP_HAS_BUILTIN(__builtin_debugtrap)
	__builtin_debugtrap();
	return;
#endif

#if defined(_WIN32)
	constexpr unsigned long EXCEPTION_BREAKPOINT = 0x80000003ul;
	RaiseException(EXCEPTION_BREAKPOINT, 0, 0, nullptr);
	return;
#endif

#if defined(__GNUC__) || defined(__clang__)
	#if defined(__x86_64__) || defined(__i386__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("int3"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__aarch64__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("brk #0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__arm__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("bkpt #0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__riscv) || defined(__riscv__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("ebreak"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc64__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("tw 31,0,0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__mips__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break 0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__loongarch__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break 0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__sparc__) || defined(__sparc)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("ta 1"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__alpha__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("bpt"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__sh__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("trapa #0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__xtensa__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break 0,0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__nios2__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break 0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__arc__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("brk"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__avr__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__m68k__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("bkpt #0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__ia64__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("break.i 0"); // NOLINT(*-no-assembler)
	return;
	#elif defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__)
	// ReSharper disable once CppDFAUnreachableCode
	__asm__ volatile("unreachable"); // NOLINT(*-no-assembler)
	return;
	#endif
#endif

#if SNAP_HAS_BUILTIN(__builtin_trap)
	// ReSharper disable once CppDFAUnreachableCode
	__builtin_trap();
#elif defined(SIGTRAP)
	// ReSharper disable once CppDFAUnreachableCode
	std::raise(SIGTRAP);
#else
	// ReSharper disable once CppDFAUnreachableCode
	std::abort();
#endif
}

SNAP_END_NAMESPACE
