#pragma once

#include <atomic>
#include <cstddef>   // std::nullptr_t
#include <memory>    // std::addressof
#include <type_traits>
#include <utility>   // std::swap, std::move

namespace snap {

// Users must specialize this for their type T:
//   struct MyType { std::atomic<unsigned> refcnt{0}; ... };
//   template<> struct intrusive_shared_ptr_traits<MyType> {
//     static std::atomic<unsigned>& get_atomic_ref_count(MyType& obj) noexcept { return obj.refcnt; }
//   };
template <class T>
struct intrusive_shared_ptr_traits;

// A lightweight intrusive shared pointer.
// The pointee T stores its own atomic refcount, exposed via intrusive_shared_ptr_traits<T>.
template <class T>
class intrusive_shared_ptr {
public:
	intrusive_shared_ptr() = default;

	explicit intrusive_shared_ptr(T* ptr) noexcept : m_ptr(ptr) {
		if (m_ptr) {
		  increment_ref_count(*m_ptr);
		}
	}

	intrusive_shared_ptr(const intrusive_shared_ptr& other) noexcept : m_ptr(other.m_ptr) {
		if (m_ptr) {
		  increment_ref_count(*m_ptr);
		}
	}

	intrusive_shared_ptr(intrusive_shared_ptr&& other) noexcept : m_ptr(other.m_ptr) {
		other.m_ptr = nullptr; // Take ownership, leave other empty
	}

	intrusive_shared_ptr& operator=(const intrusive_shared_ptr& other) noexcept {
		if (other.m_ptr != m_ptr) {
			if (other.m_ptr) {
				increment_ref_count(*other.m_ptr);
			}
			if (m_ptr) {
				decrement_ref_count(*m_ptr);
			}
			m_ptr = other.m_ptr;
		}
		return *this;
	}

	intrusive_shared_ptr& operator=(intrusive_shared_ptr&& other) noexcept {
		intrusive_shared_ptr(std::move(other)).swap(*this);
		return *this;
	}

	~intrusive_shared_ptr() {
		if (m_ptr) {
			decrement_ref_count(*m_ptr);
		}
	}

	T* operator->() const noexcept {
		return m_ptr;
	}

	T& operator*() const noexcept {
		return *m_ptr;
	}

	explicit operator bool() const noexcept {
		return m_ptr != nullptr;
	}

	void swap(intrusive_shared_ptr& other) noexcept {
		using std::swap;
		swap(m_ptr, other.m_ptr);
	}

	friend void swap(intrusive_shared_ptr& lhs, intrusive_shared_ptr& rhs) noexcept {
		lhs.swap(rhs);
	}

	friend constexpr bool operator==(const intrusive_shared_ptr& lhs, const intrusive_shared_ptr& rhs) = default;

	friend constexpr bool operator==(const intrusive_shared_ptr& ptr, std::nullptr_t) noexcept {
		return !ptr.m_ptr;
	}



private:
	T* m_ptr = nullptr;

	static void increment_ref_count(T& obj) {
		get_atomic_ref_count(obj).fetch_add(1, std::memory_order_relaxed);
	}

	static void decrement_ref_count(T& obj) {
		if (get_atomic_ref_count(obj).fetch_sub(1, std::memory_order_acq_rel) == 1) {
			delete std::addressof(obj);
		}
	}

	static decltype(auto) get_atomic_ref_count(T& obj) {
		using return_type = decltype(intrusive_shared_ptr_traits<T>::get_atomic_ref_count(obj));
		static_assert(std::is_reference_v<return_type>,
			  "intrusive_shared_ptr_traits<T>::get_atomic_ref_count must return a reference to std::atomic");
		return intrusive_shared_ptr_traits<T>::get_atomic_ref_count(obj);
	}
};

} // namespace snap
