#pragma once

#include "snap/internal/abi_namespace.hpp"

// TODO: Needs more testing on additional BSD variants / versions.

#include <array>
#include <cstddef>
#include <cstdlib>

#if defined(_MSC_VER)
// Avoid <windows.h>; declare only what we need.
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent(void);
using snap_win_dword = unsigned long;
extern "C" __declspec(dllimport) void __stdcall GetCurrentProcess(void); // not used; kept to avoid windows.h
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	#include <sys/sysctl.h>
	#include <sys/types.h>
	#include <unistd.h>
#elif defined(__unix__) || defined(__linux__) || defined(__ANDROID__)
	#include <cctype>
	#include <cstdio>
	#include <cstring>
#endif

SNAP_BEGIN_NAMESPACE

inline bool is_debugger_present() noexcept
{
#if defined(_MSC_VER)
	// Windows: nonzero means a debugger is attached.
	return IsDebuggerPresent() != 0;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	// macOS/BSD: ask the kernel for this process info, then check if it is "traced".

	// sysctl wants a mutable int* name array, so mib cannot be const here.
	std::array<int, 4> mib = { CTL_KERN, KERN_PROC, KERN_PROC_PID, (getpid()) };

	kinfo_proc info{};
	std::size_t size = sizeof(info);

	if (sysctl(mib.data(), mib.size(), &info, &size, nullptr, 0) != 0 || size == 0) { return false; }

	// The "traced" flag and field names vary by OS. If we cannot prove it, return false.

	#if defined(__APPLE__)
		#ifdef P_TRACED
	return (info.kp_proc.p_flag & P_TRACED) != 0;
		#else
	return false;
		#endif

	#elif defined(__FreeBSD__)
		#ifdef P_TRACED
	return (info.ki_flag & P_TRACED) != 0;
		#else
	return false;
		#endif

	#elif defined(__NetBSD__)
		#ifdef P_TRACED
	return (info.kp_proc.p_flag & P_TRACED) != 0;
		#else
	return false;
		#endif

	#elif defined(__OpenBSD__)
		#ifdef P_TRACED
	return (info.p_flag & P_TRACED) != 0;
		#else
	return false;
		#endif

	#else
	return false;
	#endif

#elif defined(__unix__) || defined(__linux__) || defined(__ANDROID__)
	// Linux/Android: TracerPid > 0 means a debugger is attached.

	std::FILE* f = std::fopen("/proc/self/status", "r");
	if (!f) { return false; }

	std::array<char, 256> line{};
	bool attached = false;

	while (std::fgets(line.data(), static_cast<int>(line.size()), f))
	{
		if (std::strncmp(line.data(), "TracerPid:", 10) == 0)
		{
			const char* p = line.data() + 10;
			while (*p && std::isspace(static_cast<unsigned char>(*p))) { ++p; }

			const long v = std::strtol(p, nullptr, 10);
			attached	 = (v > 0);
			break;
		}
	}

	std::fclose(f);
	return attached;

#else
	// Unknown platform: do not guess.
	return false;
#endif
}

SNAP_END_NAMESPACE
