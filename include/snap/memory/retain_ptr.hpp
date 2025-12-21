#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/compat/constexpr.hpp"
#include "snap/meta/detector.hpp"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
using std::add_pointer_t;
	using std::conditional_t;
	using std::declval;
	using std::is_convertible;
	using std::is_same;
	using std::nullptr_t;

	namespace impl
	{
		template <class T> using has_pointer = typename T::pointer;

		template <class T> using has_default_action = typename T::default_action;

		template <class T, class P> using has_use_count = decltype(T::use_count(std::declval<P>()));

	} // namespace impl

	using meta::detected_or_t;
	using meta::is_detected;

	template <class> struct retain_traits;

	template <class T> struct atomic_reference_count
	{
		template <class> friend struct retain_traits;

	protected:
		SNAP_CONSTEXPR20 atomic_reference_count() = default;

	private:
		std::atomic<long> count{ 1 };
	};

	template <class T> struct reference_count
	{
		template <class> friend struct retain_traits;

	protected:
		SNAP_CONSTEXPR20 reference_count() = default;

	private:
		long count{ 1 };
	};

	struct retain_object_t
	{
		constexpr retain_object_t() noexcept = default;
	};

	struct adopt_object_t
	{
		constexpr adopt_object_t() noexcept = default;
	};

	constexpr retain_object_t retain_object{};
	constexpr adopt_object_t adopt_object{};

	template <class T> struct retain_traits final
	{
		template <class U> using enable_if_base = std::enable_if_t<std::is_base_of_v<U, T>>;

		template <class U, class = enable_if_base<U>> static void increment(atomic_reference_count<U> *ptr) noexcept
		{
			ptr->count.fetch_add(1, std::memory_order_relaxed);
		}

		template <class U, class = enable_if_base<U>> static void decrement(atomic_reference_count<U> *ptr) noexcept
		{
			ptr->count.fetch_sub(1, std::memory_order_acq_rel);
			if (!use_count(ptr)) { delete static_cast<T *>(ptr); }
		}

		template <class U, class = enable_if_base<U>> static long use_count(atomic_reference_count<U> *ptr) noexcept
		{
			return ptr->count.load(std::memory_order_relaxed);
		}

		template <class U, class = enable_if_base<U>> static void increment(reference_count<U> *ptr) noexcept { ++ptr->count; }

		template <class U, class = enable_if_base<U>> static void decrement(reference_count<U> *ptr) noexcept
		{
			--ptr->count;
			if (!use_count(ptr)) { delete static_cast<T *>(ptr); }
		}

		template <class U, class = enable_if_base<U>> static long use_count(reference_count<U> *ptr) noexcept { return ptr->count; }
	};

	template <class T, class R = retain_traits<T>> struct retain_ptr
	{
		using element_type = T;
		using traits_type  = R;

		using pointer = detected_or_t<add_pointer_t<element_type>, impl::has_pointer, traits_type>;

		using default_action = detected_or_t<adopt_object_t, impl::has_default_action, traits_type>;

		static constexpr bool CheckAction = std::disjunction_v<std::is_same<default_action, adopt_object_t>, std::is_same<default_action, retain_object_t>>;

		static_assert(CheckAction, "traits_type::default_action must be adopt_object_t or retain_object_t");

		static constexpr auto has_use_count = is_detected<impl::has_use_count, traits_type, pointer>{};

		SNAP_CONSTEXPR20 retain_ptr(pointer ptr, retain_object_t) : retain_ptr{ ptr, adopt_object }
		{
			if (*this) { traits_type::increment(this->get()); }
		}

		SNAP_CONSTEXPR20 retain_ptr(pointer ptr, adopt_object_t) : ptr{ ptr } {}

		explicit SNAP_CONSTEXPR20 retain_ptr(pointer ptr) : retain_ptr{ ptr, default_action() } {}

		SNAP_CONSTEXPR20 retain_ptr(nullptr_t) : retain_ptr{} {}

		SNAP_CONSTEXPR20 retain_ptr(retain_ptr const &that) : ptr{ that.ptr }
		{
			if (*this) { traits_type::increment(this->get()); }
		}

		SNAP_CONSTEXPR20 retain_ptr(retain_ptr &&that) noexcept : ptr{ that.detach() } {}

		SNAP_CONSTEXPR20 retain_ptr() noexcept = default;
		SNAP_CONSTEXPR20 ~retain_ptr()
		{
			if (*this) { traits_type::decrement(this->get()); }
		}

		SNAP_CONSTEXPR20 retain_ptr &operator=(retain_ptr const &that)
		{
			retain_ptr(that).swap(*this);
			return *this;
		}

		SNAP_CONSTEXPR20 retain_ptr &operator=(retain_ptr &&that) noexcept
		{
			retain_ptr(std::move(that)).swap(*this);
			return *this;
		}

		SNAP_CONSTEXPR20 retain_ptr &operator=(nullptr_t) noexcept
		{
			this->reset(nullptr);
			return *this;
		}

		SNAP_CONSTEXPR20 void swap(retain_ptr &that) noexcept
		{
			using std::swap;
			swap(this->ptr, that.ptr);
		}

		constexpr explicit operator bool() const noexcept { return this->get(); }

		constexpr decltype(auto) operator*() const noexcept { return *this->get(); }

		constexpr pointer operator->() const noexcept { return this->get(); }

		constexpr pointer get() const noexcept { return this->ptr; }

		long use_count() const
		{
			if constexpr (has_use_count) { return this->get() ? traits_type::use_count(this->get()) : 0; }
			else { return -1; }
		}

		SNAP_CONSTEXPR20 pointer detach() noexcept
		{
			auto ptr  = this->get();
			this->ptr = pointer{};
			return ptr;
		}

		SNAP_CONSTEXPR20 void reset(pointer ptr, retain_object_t) { *this = retain_ptr(ptr, retain_object); }

		SNAP_CONSTEXPR20 void reset(pointer ptr, adopt_object_t) noexcept { *this = retain_ptr(ptr, adopt_object); }

		SNAP_CONSTEXPR20 void reset(pointer ptr) { *this = retain_ptr(ptr, default_action()); }

		SNAP_CONSTEXPR20 void reset() { reset(nullptr); }

	private:
		pointer ptr{};
	};

	// make_retain
	template <class T, class... Args> SNAP_CONSTEXPR20 retain_ptr<T> make_retain(Args &&...args)
	{
		return retain_ptr<T>(new T(std::forward<Args>(args)...));
	}

	// pointer cast helpers
	template <class To, class From, class R> constexpr retain_ptr<To, R> static_retain_pointer_cast(const retain_ptr<From, R> &p) noexcept
	{
		return retain_ptr<To, R>(static_cast<To *>(p.get()));
	}

	template <class To, class From, class R> constexpr retain_ptr<To, R> dynamic_retain_pointer_cast(const retain_ptr<From, R> &p) noexcept
	{
		return retain_ptr<To, R>(dynamic_cast<To *>(p.get()));
	}

SNAP_END_NAMESPACE

// hash support
template <class T, class R> struct std::hash<snap::retain_ptr<T, R>>
{
	std::size_t operator()(const snap::retain_ptr<T, R> &p) const noexcept { return std::hash<typename snap::retain_ptr<T, R>::pointer>()(p.get()); }
};

SNAP_BEGIN_NAMESPACE
template <class T, class R> constexpr void swap(retain_ptr<T, R> &lhs, retain_ptr<T, R> &rhs) noexcept
	{
		lhs.swap(rhs);
	}

	template <class T, class R> constexpr bool operator==(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}

	template <class T, class R> constexpr bool operator!=(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() != rhs.get();
	}

	template <class T, class R> constexpr bool operator>=(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() >= rhs.get();
	}

	template <class T, class R> constexpr bool operator<=(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() <= rhs.get();
	}

	template <class T, class R> constexpr bool operator>(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() > rhs.get();
	}

	template <class T, class R> constexpr bool operator<(retain_ptr<T, R> const &lhs, retain_ptr<T, R> const &rhs) noexcept
	{
		return lhs.get() < rhs.get();
	}

	template <class T, class R> constexpr bool operator==(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return !lhs;
	}

	template <class T, class R> constexpr bool operator!=(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return static_cast<bool>(lhs);
	}

	template <class T, class R> constexpr bool operator>=(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return !std::less<>()(lhs.get(), nullptr);
	}

	template <class T, class R> constexpr bool operator<=(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return !std::less<>()(nullptr, lhs.get());
	}

	template <class T, class R> constexpr bool operator>(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return std::less<>()(nullptr, lhs.get());
	}

	template <class T, class R> constexpr bool operator<(retain_ptr<T, R> const &lhs, nullptr_t) noexcept
	{
		return std::less<>()(lhs.get(), nullptr);
	}

	template <class T, class R> constexpr bool operator==(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return !rhs;
	}

	template <class T, class R> constexpr bool operator!=(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return static_cast<bool>(rhs);
	}

	template <class T, class R> constexpr bool operator>=(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return !std::less<>()(nullptr, rhs.get());
	}

	template <class T, class R> constexpr bool operator<=(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return !std::less<>()(rhs.get(), nullptr);
	}

	template <class T, class R> constexpr bool operator>(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return std::less<>()(rhs.get(), nullptr);
	}

	template <class T, class R> constexpr bool operator<(nullptr_t, retain_ptr<T, R> const &rhs) noexcept
	{
		return std::less<>()(nullptr, rhs.get());
	}

SNAP_END_NAMESPACE
