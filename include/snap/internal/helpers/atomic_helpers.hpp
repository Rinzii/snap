#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include "snap/internal/compat/version.hpp"

#include <array>
#include <atomic>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE

namespace internal::detail
{
	std::uint32_t parking_lot_prepare(const void* key) noexcept;
	void parking_lot_wait(const void* key, std::uint32_t gen) noexcept;
	void parking_lot_notify_one(const void* key) noexcept;
	void parking_lot_notify_all(const void* key) noexcept;

	void cpu_relax() noexcept;

	bool native_wait(const void* addr, const void* expected, std::size_t size) noexcept;
	bool native_notify_one(const void* addr) noexcept;
	bool native_notify_all(const void* addr) noexcept;

	inline constexpr int atomic_wait_spin_iters = 64;
	inline constexpr int atomic_flag_spin_iters = 64;

	constexpr std::memory_order wait_observe_order(std::memory_order order) noexcept
	{
		switch (order)
		{
		case std::memory_order_relaxed: return std::memory_order_relaxed;

		case std::memory_order_consume: [[fallthrough]];
		case std::memory_order_acquire: return std::memory_order_acquire;

		case std::memory_order_seq_cst: return std::memory_order_seq_cst;

		case std::memory_order_release: [[fallthrough]];
		case std::memory_order_acq_rel: [[fallthrough]];
		default: return std::memory_order_seq_cst;
		}
	}

	template <class T> using byte_repr_t = std::array<unsigned char, sizeof(T)>;

	template <class T, class = std::enable_if_t<std::is_trivially_copyable_v<T>>> inline byte_repr_t<T> value_bytes(const T& v) noexcept
	{
		byte_repr_t<T> out{};
		std::memcpy(out.data(), std::addressof(v), sizeof(T));
		return out;
	}

	template <class T, class = std::enable_if_t<std::is_trivially_copyable_v<T>>> inline bool value_repr_equal(const T& a, const T& b) noexcept
	{
		const auto ba = value_bytes(a);
		const auto bb = value_bytes(b);
		return std::memcmp(ba.data(), bb.data(), sizeof(T)) == 0;
	}

	template <class, class, class = void> struct native_object_wait_ok : std::false_type
	{
	};

	template <class A, class V> struct native_object_wait_ok<
		A,
		V,
		std::void_t<std::enable_if_t<std::is_trivially_copyable_v<V> && std::is_trivially_copyable_v<A> && sizeof(A) == sizeof(V) && alignof(A) >= alignof(V)>>>
		: std::true_type
	{
	};

	template <class, class, class = std::void_t<>> struct has_wait : std::false_type
	{
	};

	template <class A, class V> struct has_wait<A, V, std::void_t<decltype(std::declval<A&>().wait(std::declval<V>(), std::memory_order_relaxed))>>
		: std::true_type
	{
	};

	template <class, class = std::void_t<>> struct has_notify_one : std::false_type
	{
	};

	template <class A> struct has_notify_one<A, std::void_t<decltype(std::declval<A&>().notify_one())>> : std::true_type
	{
	};

	template <class, class = std::void_t<>> struct has_notify_all : std::false_type
	{
	};

	template <class A> struct has_notify_all<A, std::void_t<decltype(std::declval<A&>().notify_all())>> : std::true_type
	{
	};

	template <class, class = std::void_t<>> struct has_load : std::false_type
	{
	};

	template <class A> struct has_load<A, std::void_t<decltype(std::declval<A&>().load(std::memory_order_relaxed))>> : std::true_type
	{
	};

	template <class, class = std::void_t<>> struct has_test : std::false_type
	{
	};

	template <class A> struct has_test<A, std::void_t<decltype(std::declval<A&>().test(std::memory_order_relaxed))>> : std::true_type
	{
	};

	template <class AtomicLike> auto read_value(AtomicLike& a, std::memory_order order) noexcept
		-> std::enable_if_t<has_load<AtomicLike>::value, decltype(a.load(order))>
	{
		return a.load(order);
	}

	template <class AtomicLike> auto read_value(AtomicLike& a, std::memory_order order) noexcept
		-> std::enable_if_t<!has_load<AtomicLike>::value && has_test<AtomicLike>::value, decltype(a.test(order))>
	{
		return a.test(order);
	}

	template <class, class, class = void> struct can_fallback_wait : std::false_type
	{
	};

	template <class AtomicLike, class Value> struct can_fallback_wait<
		AtomicLike,
		Value,
		std::void_t<
			decltype(read_value(std::declval<AtomicLike&>(), std::memory_order_relaxed)),
			std::enable_if_t<std::is_trivially_copyable_v<std::decay_t<decltype(read_value(std::declval<AtomicLike&>(), std::memory_order_relaxed))>> &&
							 std::is_constructible_v<std::decay_t<decltype(read_value(std::declval<AtomicLike&>(), std::memory_order_relaxed))>, Value>>>>
		: std::true_type
	{
	};

	template <class, class = std::void_t<>> struct is_atomic_like : std::false_type
	{
	};

	template <class AtomicLike> struct is_atomic_like<AtomicLike,
													  std::void_t<std::enable_if_t<has_load<AtomicLike>::value || has_test<AtomicLike>::value ||
																				   has_notify_one<AtomicLike>::value || has_notify_all<AtomicLike>::value>>>
		: std::true_type
	{
	};
} // namespace internal::detail

namespace internal
{
	template <class AtomicLike,
			  class Value,
			  std::enable_if_t<detail::has_wait<AtomicLike, Value>::value || detail::can_fallback_wait<AtomicLike, Value>::value, int> = 0>
	void atomic_wait(AtomicLike& a, Value expected, [[maybe_unused]] std::memory_order order = std::memory_order_seq_cst) noexcept
	{
		if constexpr (detail::has_wait<AtomicLike, Value>::value) { a.wait(expected, order); }
		else
		{
			const std::memory_order observe = detail::wait_observe_order(order);

			using observed_t		= std::decay_t<decltype(detail::read_value(a, std::memory_order_relaxed))>;
			const auto expected_obs = static_cast<observed_t>(expected);

			auto is_expected = [&](std::memory_order mo) noexcept -> bool
			{
				const observed_t cur = detail::read_value(a, mo);
				return detail::value_repr_equal(cur, expected_obs);
			};

			for (;;)
			{
				if (!is_expected(observe)) { return; }

				for (int i = 0; i < detail::atomic_wait_spin_iters; ++i)
				{
					detail::cpu_relax();

					if (!is_expected(std::memory_order_relaxed))
					{
						if (!is_expected(observe)) { return; }
					}
				}

				if constexpr (detail::native_object_wait_ok<AtomicLike, observed_t>::value)
				{
					const auto repr = detail::value_bytes(expected_obs);
					if (detail::native_wait(std::addressof(a), repr.data(), repr.size())) { continue; }
				}

				const std::uint32_t g = detail::parking_lot_prepare(std::addressof(a));

				if (!is_expected(observe)) { return; }

				detail::parking_lot_wait(std::addressof(a), g);
			}
		}
	}

	template <class AtomicLike, std::enable_if_t<detail::is_atomic_like<AtomicLike>::value, int> = 0> void atomic_notify_one(AtomicLike& a) noexcept
	{
		if constexpr (detail::has_notify_one<AtomicLike>::value) { a.notify_one(); }
		else
		{
			using observed_t = std::decay_t<decltype(detail::read_value(a, std::memory_order_relaxed))>;
			if constexpr (detail::native_object_wait_ok<AtomicLike, observed_t>::value)
			{
				[[maybe_unused]] const bool ok = detail::native_notify_one(std::addressof(a));
			}
			detail::parking_lot_notify_one(std::addressof(a));
		}
	}

	template <class AtomicLike, std::enable_if_t<detail::is_atomic_like<AtomicLike>::value, int> = 0> void atomic_notify_all(AtomicLike& a) noexcept
	{
		if constexpr (detail::has_notify_all<AtomicLike>::value) { a.notify_all(); }
		else
		{
			using observed_t = std::decay_t<decltype(detail::read_value(a, std::memory_order_relaxed))>;
			if constexpr (detail::native_object_wait_ok<AtomicLike, observed_t>::value)
			{
				[[maybe_unused]] const bool ok = detail::native_notify_all(std::addressof(a));
			}
			detail::parking_lot_notify_all(std::addressof(a));
		}
	}

	inline void atomic_flag_lock(std::atomic_flag& f) noexcept
	{
		for (;;)
		{
			if (!f.test_and_set(std::memory_order_acquire)) { return; }

#if defined(__cpp_lib_atomic_flag_test) && (__cpp_lib_atomic_flag_test >= 201907L)
			for (int i = 0; i < detail::atomic_flag_spin_iters; ++i)
			{
				while (f.test(std::memory_order_relaxed)) { detail::cpu_relax(); }
				if (!f.test_and_set(std::memory_order_acquire)) { return; }
			}
#else
			for (int i = 0; i < detail::atomic_flag_spin_iters; ++i)
			{
				detail::cpu_relax();
				if (!f.test_and_set(std::memory_order_acquire)) { return; }
			}
#endif

			const std::uint32_t g = detail::parking_lot_prepare(std::addressof(f));

			if (!f.test_and_set(std::memory_order_acquire)) { return; }

			detail::parking_lot_wait(std::addressof(f), g);
		}
	}

	inline void atomic_flag_unlock(std::atomic_flag& f) noexcept
	{
		f.clear(std::memory_order_release);
		detail::parking_lot_notify_all(std::addressof(f));
	}
} // namespace internal

SNAP_END_NAMESPACE
