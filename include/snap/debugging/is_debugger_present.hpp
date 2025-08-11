#pragma once

// TODO: Should be complete but needs more testing on other environments.

#include <cstddef>
#include <cstdlib>
#include <cstdint>

#if defined(_MSC_VER)
  // Avoid including <windows.h>; declare the minimal Win32 APIs we need.
  extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent(void);
  using snap_win_dword = unsigned long;
  extern "C" __declspec(dllimport) void __stdcall GetCurrentProcess(void); // just to avoid windows.h; not actually used
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
  #include <unistd.h>
#elif defined(__unix__) || defined(__linux__) || defined(__ANDROID__)
  #include <cstdio>
  #include <cstring>
  #include <cctype>
#endif

namespace snap {

inline bool is_debugger_present() noexcept {
#if defined(_MSC_VER)
    // Works for MSVC and clang-cl on Windows.
    // Returns nonzero if a debugger is attached.
    return IsDebuggerPresent() != 0;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    // BSD/macOS: query process info via sysctl and check the traced flag.
    // macOS: kinfo_proc.kp_proc.p_flag & P_TRACED
    // FreeBSD: kinfo_proc.ki_flag & P_TRACED
    // NetBSD/OpenBSD have similar structures/flags; if the struct size
    // or flag names differ, this may conservatively return false.

    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, static_cast<int>(getpid()) };
    // The type of kinfo_proc differs across BSDs; declare a buffer and size,
    // then check whichever flag field is present via macros.
    struct kinfo_proc info{};
    std::size_t size = sizeof(info);
    if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0 || size == 0) {
        return false;
    }

    // Try the most common layouts in a portable way.
    // macOS uses kp_proc.p_flag
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
    #elif defined(__NetBSD__) || defined(__OpenBSD__)
      // These platforms also expose a traced flag; names vary with version.
      // If unavailable at compile time, conservatively return false.
      #ifdef P_TRACED
        // NetBSD uses kp_proc.p_flag; OpenBSD uses p_flag within kinfo_proc.
        // Try both guardedly.
        #if defined(__NetBSD__)
          return (info.kp_proc.p_flag & P_TRACED) != 0;
        #elif defined(__OpenBSD__)
          return (info.p_flag & P_TRACED) != 0;
        #else
          return false;
        #endif
      #else
        return false;
      #endif
    #else
      return false;
    #endif

#elif defined(__unix__) || defined(__linux__) || defined(__ANDROID__)
    // Linux/Android: read /proc/self/status and parse TracerPid
    std::FILE* f = std::fopen("/proc/self/status", "r");
    if (!f) return false;

    char line[256];
    bool attached = false;
    while (std::fgets(line, sizeof line, f)) {
        // Look for: "TracerPid:\t<N>"
        if (std::strncmp(line, "TracerPid:", 10) == 0) {
            const char* p = line + 10;
            while (*p && std::isspace(static_cast<unsigned char>(*p))) ++p;
            long v = std::strtol(p, nullptr, 10);
            attached = (v > 0);
            break;
        }
    }
    std::fclose(f);
    return attached;
#else
    // Unknown platform: conservatively return false.
    return false;
#endif
}

} // namespace snap
