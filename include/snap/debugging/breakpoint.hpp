#pragma once

#include "snap/internal/abi_namespace.hpp"

// TODO: Should be complete but needs more testing on other environments.

#include <csignal> // std::raise, SIGTRAP
#include <cstdlib> // std::abort

#if defined(_MSC_VER)
	#include <intrin.h> // __debugbreak
#endif

#if defined(__EMSCRIPTEN__)
	#include <emscripten/emscripten.h> // emscripten_debugger
#endif

SNAP_BEGIN_NAMESPACE
inline void breakpoint() noexcept
{
#if defined(_MSC_VER)
	// MSVC / clang-cl
	__debugbreak();

#elif defined(__EMSCRIPTEN__)
	// WebAssembly (Emscripten): triggers a 'debugger;' statement in DevTools
	emscripten_debugger();

#elif defined(__has_builtin)
	#if __has_builtin(__builtin_debugtrap)
	__builtin_debugtrap();
	#elif defined(_WIN32)
	// Windows fallback via SEH breakpoint
	extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long, unsigned long, unsigned long, const void *);
	constexpr unsigned long EXCEPTION_BREAKPOINT = 0x80000003ul;
	RaiseException(EXCEPTION_BREAKPOINT, 0, 0, nullptr);
	#elif __has_builtin(__builtin_trap)
	__builtin_trap();
	#else
		// Arch-specific traps, else signal/abort
		#if defined(__x86_64__) || defined(__i386__)
	__asm__ volatile("int3");
		#elif defined(__aarch64__)
	__asm__ volatile("brk #0");
		#elif defined(__arm__)
	__asm__ volatile("bkpt #0");
		#elif defined(__riscv)
	__asm__ volatile("ebreak");
		#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
	__asm__ volatile("tw 31,0,0");
		#elif defined(__mips__)
	__asm__ volatile("break 0");
		#elif defined(SIGTRAP)
	std::raise(SIGTRAP);
		#else
	std::abort();
		#endif
	#endif

#else // compiler doesn't provide __has_builtin
	#if defined(__GNUC__) || defined(__clang__)
	// Prefer builtin first
	__builtin_trap(); // fallback if no debugtrap probe available
	#elif defined(_WIN32)
	extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long, unsigned long, unsigned long, const void *);
	constexpr unsigned long EXCEPTION_BREAKPOINT = 0x80000003ul;
	RaiseException(EXCEPTION_BREAKPOINT, 0, 0, nullptr);
	#elif defined(SIGTRAP)
	std::raise(SIGTRAP);
	#else
	std::abort();
	#endif
#endif
}

SNAP_END_NAMESPACE
