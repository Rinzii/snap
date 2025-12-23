// Must be included first
#include "snap/internal/helpers/atomic_helpers.hpp"

#include <array>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>

#if defined(_WIN32)
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <windows.h>
#elif defined(__APPLE__)
	#if __has_include(<sys/ulock.h>)
		#include <sys/ulock.h>
		#define SNAP_HAS_APPLE_ULOCK 1
	#else
		#define SNAP_HAS_APPLE_ULOCK 0
	#endif
#elif defined(__linux__) || defined(__ANDROID__)
	#include <linux/futex.h>
	#include <sys/syscall.h>
	#include <unistd.h>

	#include <cerrno>
	#include <climits>
#elif defined(__FreeBSD__)
	#include <sys/types.h>
	#include <sys/umtx.h>

	#include <cerrno>
	#include <climits>
#elif defined(__OpenBSD__)
	#include <sys/futex.h>
	#include <sys/time.h>

	#include <cerrno>
	#include <climits>
#elif defined(__NetBSD__)
	#include <sys/futex.h>
	#include <sys/syscall.h>
	#include <unistd.h>

	#include <cerrno>
	#include <climits>
#endif

#if defined(_MSC_VER)
	#include <intrin.h>
	#if defined(_M_IX86) || defined(_M_X64)
		#include <immintrin.h>
	#endif
#endif

SNAP_BEGIN_NAMESPACE

namespace internal::detail
{
	namespace
	{
#if defined(_MSC_VER)
		std::uint32_t atomic_load_u32_relaxed(const std::uint32_t* p) noexcept
		{
			return static_cast<std::uint32_t>(_InterlockedCompareExchange(reinterpret_cast<volatile long*>(const_cast<std::uint32_t*>(p)), 0, 0));
		}

		std::uint32_t atomic_fetch_add_u32_relaxed(std::uint32_t* p, std::uint32_t v) noexcept
		{
			return static_cast<std::uint32_t>(_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(p), static_cast<long>(v)));
		}
#else
		std::uint32_t atomic_load_u32_relaxed(const std::uint32_t* p) noexcept
		{
			return __atomic_load_n(p, __ATOMIC_RELAXED);
		}

		std::uint32_t atomic_fetch_add_u32_relaxed(std::uint32_t* p, std::uint32_t v) noexcept
		{
			return __atomic_fetch_add(p, v, __ATOMIC_RELAXED);
		}
#endif

		bool aligned_for(std::size_t align, const void* p) noexcept
		{
			const auto v = reinterpret_cast<std::uintptr_t>(p); // NOLINT(*-pro-type-reinterpret-cast)
			return (v & (align - 1)) == 0;
		}

#if defined(_WIN32)
		using wait_on_address_fn		  = BOOL(WINAPI*)(volatile void*, void*, SIZE_T, DWORD);
		using wake_by_address_single_fn = VOID(WINAPI*)(void*);
		using wake_by_address_all_fn	  = VOID(WINAPI*)(void*);

		wait_on_address_fn g_wait_on_address			  = nullptr;
		wake_by_address_single_fn g_wake_by_address_single = nullptr;
		wake_by_address_all_fn g_wake_by_address_all		  = nullptr;

		std::once_flag g_wait_address_once;
		bool g_wait_address_available = false;

		void init_wait_address_functions() noexcept
		{
			const HMODULE kernel = ::GetModuleHandleW(L"kernel32.dll");
			if (kernel == nullptr) { return; }

			g_wait_on_address = reinterpret_cast<wait_on_address_fn>(::GetProcAddress(kernel, "WaitOnAddress"));
			g_wake_by_address_single =
				reinterpret_cast<wake_by_address_single_fn>(::GetProcAddress(kernel, "WakeByAddressSingle"));
			g_wake_by_address_all = reinterpret_cast<wake_by_address_all_fn>(::GetProcAddress(kernel, "WakeByAddressAll"));

			g_wait_address_available =
				g_wait_on_address != nullptr && g_wake_by_address_single != nullptr && g_wake_by_address_all != nullptr;
		}

		bool win32_wait_functions_available() noexcept
		{
			std::call_once(g_wait_address_once, init_wait_address_functions);
			return g_wait_address_available;
		}
#endif
	} // namespace

	void cpu_relax() noexcept
	{
#if defined(_MSC_VER)
	#if defined(_M_IX86) || defined(_M_X64)
		_mm_pause();
	#elif defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
		__yield();
	#else
		std::atomic_signal_fence(std::memory_order_seq_cst);
	#endif
#elif defined(__clang__) || defined(__GNUC__)
	#if defined(__i386__) || defined(__x86_64__)
		__asm__ __volatile__("pause" ::: "memory"); // NOLINT(*-no-assembler)
	#elif defined(__aarch64__) || defined(__arm__)
		__asm__ __volatile__("yield" ::: "memory"); // NOLINT(*-no-assembler)
	#elif defined(__riscv)
		#if defined(__riscv_zihintpause)
		__asm__ __volatile__("pause" ::: "memory"); // NOLINT(*-no-assembler)
		#else
		__asm__ __volatile__("nop" ::: "memory"); // NOLINT(*-no-assembler)
		#endif
	#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(__powerpc64__) || defined(__ppc64__)
		__asm__ __volatile__("or 27,27,27" ::: "memory"); // NOLINT(*-no-assembler)
	#elif defined(__s390__) || defined(__s390x__)
		__asm__ __volatile__("nopr 0" ::: "memory"); // NOLINT(*-no-assembler)
	#else
		std::atomic_signal_fence(std::memory_order_seq_cst);
	#endif
#else
		std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
	}

	bool native_wait(const void* addr, const void* expected, std::size_t size) noexcept
	{
#if defined(_WIN32)
		if (size != 1 && size != 2 && size != 4 && size != 8) { return false; }
		if (!aligned_for(size, addr)) { return false; }
		if (!win32_wait_functions_available()) { return false; }
		[[maybe_unused]] const auto rc = g_wait_on_address(const_cast<void*>(addr), const_cast<void*>(expected), size, INFINITE);
		return rc != 0;
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
		if (size != 4) { return false; }
		if (!aligned_for(4, addr)) { return false; }
		std::uint32_t old = 0;
		std::memcpy(std::addressof(old), expected, 4);
		[[maybe_unused]] const int rc = __ulock_wait(UL_COMPARE_AND_WAIT, const_cast<void*>(addr), static_cast<std::uint64_t>(old), 0);
		return rc == 0;
#elif defined(__linux__) || defined(__ANDROID__)
		if (size != 4) { return false; }
		if (!aligned_for(4, addr)) { return false; }
		std::uint32_t old = 0;
		std::memcpy(std::addressof(old), expected, 4);
		auto* p = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		for (;;)
		{
			const long rc = ::syscall(SYS_futex, p, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, static_cast<int>(old), nullptr, nullptr, 0);
			if (rc == 0) { return true; }
			if (errno == EAGAIN) { return true; }
			if (errno == EINTR) { continue; }
			return true;
		}
#elif defined(__FreeBSD__)
		if (size != 4) { return false; }
		if (!aligned_for(4, addr)) { return false; }
		std::uint32_t old = 0;
		std::memcpy(std::addressof(old), expected, 4);
		void* p = const_cast<void*>(addr);
		for (;;)
		{
			const int rc = _umtx_op(p, UMTX_OP_WAIT_UINT, static_cast<u_long>(old), nullptr, nullptr);
			if (rc == 0) { return true; }
			if (errno == EBUSY) { return true; }
			if (errno == EINTR) { continue; }
			return true;
		}
#elif defined(__OpenBSD__)
		if (size != 4) { return false; }
		if (!aligned_for(4, addr)) { return false; }
		std::uint32_t old = 0;
		std::memcpy(std::addressof(old), expected, 4);
		auto* p = reinterpret_cast<volatile std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		for (;;)
		{
			const int rc = ::futex(p, FUTEX_WAIT, static_cast<int>(old), nullptr, nullptr);
			if (rc == 0) { return true; }
			if (errno == EAGAIN) { return true; }
			if (errno == EINTR) { continue; }
			return true;
		}
#elif defined(__NetBSD__)
		if (size != 4) { return false; }
		if (!aligned_for(4, addr)) { return false; }
		std::uint32_t old = 0;
		std::memcpy(std::addressof(old), expected, 4);
		auto* p = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		for (;;)
		{
			const long rc = ::syscall(SYS___futex, p, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, static_cast<int>(old), nullptr, nullptr, 0, 0);
			if (rc == 0) { return true; }
			if (errno == EAGAIN) { return true; }
			if (errno == EINTR) { continue; }
			return true;
		}
#else
		[[maybe_unused]] const void* u_addr		  = addr;
		[[maybe_unused]] const void* u_expected	  = expected;
		[[maybe_unused]] const std::size_t u_size = size;
		return false;
#endif
	}

	bool native_notify_one(const void* addr) noexcept
	{
#if defined(_WIN32)
		if (!win32_wait_functions_available()) { return false; }
		g_wake_by_address_single(const_cast<void*>(addr));
		return true;
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
		[[maybe_unused]] const int rc = __ulock_wake(UL_COMPARE_AND_WAIT, const_cast<void*>(addr), 0);
		return rc == 0;
#elif defined(__linux__) || defined(__ANDROID__)
		auto* p						   = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const long rc = ::syscall(SYS_futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);
		return rc >= 0;
#elif defined(__FreeBSD__)
		void* p						  = const_cast<void*>(addr);
		[[maybe_unused]] const int rc = _umtx_op(p, UMTX_OP_WAKE, 1, nullptr, nullptr);
		return rc == 0;
#elif defined(__OpenBSD__)
		auto* p						  = reinterpret_cast<volatile std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const int rc = ::futex(p, FUTEX_WAKE, 1, nullptr, nullptr);
		return rc == 0;
#elif defined(__NetBSD__)
		auto* p						   = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const long rc = ::syscall(SYS___futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0, 0);
		return rc >= 0;
#else
		[[maybe_unused]] const void* u_addr = addr;
		return false;
#endif
	}

	bool native_notify_all(const void* addr) noexcept
	{
#if defined(_WIN32)
		if (!win32_wait_functions_available()) { return false; }
		g_wake_by_address_all(const_cast<void*>(addr));
		return true;
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
		[[maybe_unused]] const int rc = __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, const_cast<void*>(addr), 0);
		return rc == 0;
#elif defined(__linux__) || defined(__ANDROID__)
		auto* p						   = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const long rc = ::syscall(SYS_futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT_MAX, nullptr, nullptr, 0);
		return rc >= 0;
#elif defined(__FreeBSD__)
		void* p						  = const_cast<void*>(addr);
		[[maybe_unused]] const int rc = _umtx_op(p, UMTX_OP_WAKE, INT_MAX, nullptr, nullptr);
		return rc == 0;
#elif defined(__OpenBSD__)
		auto* p						  = reinterpret_cast<volatile std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const int rc = ::futex(p, FUTEX_WAKE, INT_MAX, nullptr, nullptr);
		return rc == 0;
#elif defined(__NetBSD__)
		auto* p						   = reinterpret_cast<std::uint32_t*>(const_cast<void*>(addr)); // NOLINT(*-pro-type-reinterpret-cast)
		[[maybe_unused]] const long rc = ::syscall(SYS___futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT_MAX, nullptr, nullptr, 0, 0);
		return rc >= 0;
#else
		[[maybe_unused]] const void* u_addr = addr;
		return false;
#endif
	}

	namespace
	{
		struct bucket
		{
			alignas(4) std::uint32_t gen = 0;
			std::mutex m;
			std::condition_variable cv;
		};

		constexpr std::size_t bucket_count = 256;
		std::array<bucket, bucket_count> buckets; // NOLINT(*-avoid-non-const-global-variables)

		bucket& bucket_for(const void* key) noexcept
		{
			const auto v		  = reinterpret_cast<std::uintptr_t>(key); // NOLINT(*-pro-type-reinterpret-cast)
			const std::size_t idx = (v >> 4) & (bucket_count - 1);
			return buckets[idx]; // NOLINT(*-pro-bounds-constant-array-index)
		}

		void wait_u32(bucket& b, std::uint32_t expected) noexcept
		{
#if defined(_WIN32)
			if (win32_wait_functions_available())
			{
				[[maybe_unused]] const auto rc = g_wait_on_address(
					const_cast<void*>(reinterpret_cast<const void*>(std::addressof(b.gen))), std::addressof(expected), sizeof(expected), INFINITE);
				return;
			}
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
			[[maybe_unused]] const int rc =
				__ulock_wait(UL_COMPARE_AND_WAIT, reinterpret_cast<void*>(std::addressof(b.gen)), static_cast<std::uint64_t>(expected), 0);
			return;
#elif defined(__linux__) || defined(__ANDROID__)
			auto* p = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			for (;;)
			{
				const long rc = ::syscall(SYS_futex, p, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, static_cast<int>(expected), nullptr, nullptr, 0);
				if (rc == 0) { return; }
				if (errno == EAGAIN) { return; }
				if (errno == EINTR) { continue; }
				return;
			}
#elif defined(__FreeBSD__)
			void* p = reinterpret_cast<void*>(std::addressof(b.gen));
			for (;;)
			{
				const int rc = _umtx_op(p, UMTX_OP_WAIT_UINT, static_cast<u_long>(expected), nullptr, nullptr);
				if (rc == 0) { return; }
				if (errno == EBUSY) { return; }
				if (errno == EINTR) { continue; }
				return;
			}
#elif defined(__OpenBSD__)
			auto* p = reinterpret_cast<volatile std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			for (;;)
			{
				const int rc = ::futex(p, FUTEX_WAIT, static_cast<int>(expected), nullptr, nullptr);
				if (rc == 0) { return; }
				if (errno == EAGAIN) { return; }
				if (errno == EINTR) { continue; }
				return;
			}
#elif defined(__NetBSD__)
			auto* p = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			for (;;)
			{
				const long rc = ::syscall(SYS___futex, p, FUTEX_WAIT | FUTEX_PRIVATE_FLAG, static_cast<int>(expected), nullptr, nullptr, 0, 0);
				if (rc == 0) { return; }
				if (errno == EAGAIN) { return; }
				if (errno == EINTR) { continue; }
				return;
			}
#endif
			std::unique_lock lk(b.m);
			b.cv.wait(lk, [&] { return atomic_load_u32_relaxed(std::addressof(b.gen)) != expected; });
		}

		void wake_one_u32(bucket& b) noexcept
		{
#if defined(_WIN32)
			if (win32_wait_functions_available())
			{
				g_wake_by_address_single(reinterpret_cast<void*>(std::addressof(b.gen)));
				return;
			}
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
			[[maybe_unused]] const int rc = __ulock_wake(UL_COMPARE_AND_WAIT, reinterpret_cast<void*>(std::addressof(b.gen)), 0);
			return;
#elif defined(__linux__) || defined(__ANDROID__)
			auto* p						   = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const long rc = ::syscall(SYS_futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0);
			return;
#elif defined(__FreeBSD__)
			void* p						  = reinterpret_cast<void*>(std::addressof(b.gen));
			[[maybe_unused]] const int rc = _umtx_op(p, UMTX_OP_WAKE, 1, nullptr, nullptr);
			return;
#elif defined(__OpenBSD__)
			auto* p						  = reinterpret_cast<volatile std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const int rc = ::futex(p, FUTEX_WAKE, 1, nullptr, nullptr);
			return;
#elif defined(__NetBSD__)
			auto* p						   = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const long rc = ::syscall(SYS___futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, nullptr, nullptr, 0, 0);
			return;
#endif
			b.cv.notify_one();
		}

		void wake_all_u32(bucket& b) noexcept
		{
#if defined(_WIN32)
			if (win32_wait_functions_available())
			{
				g_wake_by_address_all(reinterpret_cast<void*>(std::addressof(b.gen)));
				return;
			}
#elif defined(__APPLE__) && SNAP_HAS_APPLE_ULOCK
			[[maybe_unused]] const int rc = __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, reinterpret_cast<void*>(std::addressof(b.gen)), 0);
			return;
#elif defined(__linux__) || defined(__ANDROID__)
			auto* p						   = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const long rc = ::syscall(SYS_futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT_MAX, nullptr, nullptr, 0);
			return;
#elif defined(__FreeBSD__)
			void* p						  = reinterpret_cast<void*>(std::addressof(b.gen));
			[[maybe_unused]] const int rc = _umtx_op(p, UMTX_OP_WAKE, INT_MAX, nullptr, nullptr);
			return;
#elif defined(__OpenBSD__)
			auto* p						  = reinterpret_cast<volatile std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const int rc = ::futex(p, FUTEX_WAKE, INT_MAX, nullptr, nullptr);
			return;
#elif defined(__NetBSD__)
			auto* p						   = reinterpret_cast<std::uint32_t*>(std::addressof(b.gen)); // NOLINT(*-pro-type-reinterpret-cast)
			[[maybe_unused]] const long rc = ::syscall(SYS___futex, p, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, INT_MAX, nullptr, nullptr, 0, 0);
			return;
#endif
			b.cv.notify_all();
		}
	} // namespace

	std::uint32_t parking_lot_prepare(const void* key) noexcept
	{
		auto& b = bucket_for(key);
		return atomic_load_u32_relaxed(std::addressof(b.gen));
	}

	void parking_lot_wait(const void* key, std::uint32_t gen) noexcept
	{
		auto& b = bucket_for(key);

		for (;;)
		{
			if (atomic_load_u32_relaxed(std::addressof(b.gen)) != gen) { return; }
			wait_u32(b, gen);
			if (atomic_load_u32_relaxed(std::addressof(b.gen)) != gen) { return; }
		}
	}

	void parking_lot_notify_one(const void* key) noexcept
	{
		auto& b									  = bucket_for(key);
		[[maybe_unused]] const std::uint32_t prev = atomic_fetch_add_u32_relaxed(std::addressof(b.gen), 1);
		wake_one_u32(b);
	}

	void parking_lot_notify_all(const void* key) noexcept
	{
		auto& b									  = bucket_for(key);
		[[maybe_unused]] const std::uint32_t prev = atomic_fetch_add_u32_relaxed(std::addressof(b.gen), 1);
		wake_all_u32(b);
	}
} // namespace internal::detail

SNAP_END_NAMESPACE
