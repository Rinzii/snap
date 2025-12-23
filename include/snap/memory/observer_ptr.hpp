#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef>	   // std::nullptr_t
#include <functional>  // std::hash, std::less
#include <type_traits> // std::is_reference_v, std::enable_if_t, std::is_convertible_v, std::add_lvalue_reference_t
#include <utility>	   // std::swap

SNAP_BEGIN_NAMESPACE
template <class W> class observer_ptr
{
	static_assert(!std::is_reference_v<W>, "observer_ptr<W>: W must not be a reference type");

public:
	using element_type = W;

	// constructors
	constexpr observer_ptr() noexcept = default;
	constexpr observer_ptr(std::nullptr_t) noexcept : m_ptr(nullptr) {}
	constexpr explicit observer_ptr(element_type* p) noexcept : m_ptr(p) {}

	template <class W2, class = std::enable_if_t<std::is_convertible_v<W2*, element_type*>>> constexpr observer_ptr(observer_ptr<W2> other) noexcept
		: m_ptr(other.get())
	{
	}

	observer_ptr(const observer_ptr&)	  = default;
	observer_ptr(observer_ptr&&) noexcept = default;

	// assignment
	observer_ptr& operator=(const observer_ptr&)	 = default;
	observer_ptr& operator=(observer_ptr&&) noexcept = default;

	// modifiers
	constexpr element_type* release() noexcept
	{
		element_type* p = m_ptr;
		m_ptr			= nullptr;
		return p;
	}

	constexpr void reset(element_type* p = nullptr) noexcept { m_ptr = p; }
	constexpr void swap(observer_ptr& other) noexcept { std::swap(m_ptr, other.m_ptr); }

	// observers
	constexpr element_type* get() const noexcept { return m_ptr; }
	constexpr explicit operator bool() const noexcept { return m_ptr != nullptr; }

	// UB if get() == nullptr (same as raw pointer)
	constexpr std::add_lvalue_reference_t<element_type> operator*() const { return *m_ptr; }
	constexpr element_type* operator->() const noexcept { return m_ptr; }

	// conversion
	constexpr explicit operator element_type*() const noexcept { return m_ptr; }

private:
	element_type* m_ptr = nullptr;
};

// make_observer
template <class W> constexpr observer_ptr<W> make_observer(W* p) noexcept
{
	return observer_ptr<W>(p);
}

// comparisons with other observer_ptr
template <class W1, class W2> bool operator==(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return a.get() == b.get();
}
template <class W1, class W2> bool operator!=(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return !(a == b);
}
template <class W1, class W2> bool operator<(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return std::less<>{}(a.get(), b.get());
}
template <class W1, class W2> bool operator>(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return b < a;
}
template <class W1, class W2> bool operator<=(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return !(b < a);
}
template <class W1, class W2> bool operator>=(const observer_ptr<W1>& a, const observer_ptr<W2>& b)
{
	return !(a < b);
}

// comparisons with nullptr
template <class W> bool operator==(const observer_ptr<W>& p, std::nullptr_t) noexcept
{
	return !p;
}
template <class W> bool operator==(std::nullptr_t, const observer_ptr<W>& p) noexcept
{
	return !p;
}
template <class W> bool operator!=(const observer_ptr<W>& p, std::nullptr_t) noexcept
{
	return static_cast<bool>(p);
}
template <class W> bool operator!=(std::nullptr_t, const observer_ptr<W>& p) noexcept
{
	return static_cast<bool>(p);
}

// free swap (ADL)
template <class W> void swap(observer_ptr<W>& a, observer_ptr<W>& b) noexcept
{
	a.swap(b);
}

SNAP_END_NAMESPACE

// std::hash specialization for SNAP_NAMESPACE::observer_ptr
template <class T> struct std::hash<SNAP_NAMESPACE::observer_ptr<T>>
{
	size_t operator()(const SNAP_NAMESPACE::observer_ptr<T>& p) const noexcept { return std::hash<T*>{}(p.get()); }
}; // namespace std
