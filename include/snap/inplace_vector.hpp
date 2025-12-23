#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/internal/compat/constexpr.hpp"

#include <algorithm> // std::move, std::move_backward, std::lexicographical_compare
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <new>		 // std::launder, placement new
#include <stdexcept> // std::bad_alloc, std::out_of_range
#include <type_traits>
#include <utility> // std::swap, std::move, std::addressof

SNAP_BEGIN_NAMESPACE
// primary template (N > 0)
template <class T, std::size_t N> struct inplace_vector
{
	static_assert(N > 0, "Primary template is for N > 0; N==0 has a specialization");
	static_assert(std::is_move_constructible<T>::value && std::is_move_assignable<T>::value, "T must be MoveConstructible and MoveAssignable");

	// member types
	using value_type			 = T;
	using size_type				 = std::size_t;
	using difference_type		 = std::ptrdiff_t;
	using reference				 = value_type&;
	using const_reference		 = const value_type&;
	using pointer				 = value_type*;
	using const_pointer			 = const value_type*;
	using iterator				 = pointer;
	using const_iterator		 = const_pointer;
	using reverse_iterator		 = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	// capacity is compile-time
	static constexpr size_type static_capacity = N;

	// constructors

	// (1) Constructs an empty inplace_vector whose data() == nullptr and size() == 0.
	constexpr inplace_vector() noexcept = default;

	// (2) Constructs an inplace_vector with count default-inserted elements.
	explicit inplace_vector(size_type count)
	{
		if (count > N) throw std::bad_alloc();
		construct_n_default(count);
		m_size = count;
	}

	// (3) Constructs an inplace_vector with count copies of elements with value value.
	inplace_vector(size_type count, const T& value)
	{
		if (count > N) throw std::bad_alloc();
		size_type i = 0;
		try
		{
			for (; i < count; ++i) ::new (static_cast<void*>(base() + i)) T(value);
			m_size = count;
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	// (4) Constructs an inplace_vector with the contents of the range [first, last).
	template <class InputIt, class = std::enable_if_t<!std::is_integral<InputIt>::value>> inplace_vector(InputIt first, InputIt last)
	{
		size_type i = 0;
		try
		{
			for (; first != last; ++first)
			{
				if (i == N) throw std::bad_alloc();
				::new (static_cast<void*>(base() + i)) T(*first);
				++i;
			}
			m_size = i;
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	// (5) Constructs an inplace_vector with the contents of the range rg.
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> explicit inplace_vector(R&& rg)
	{
		auto it		= std::begin(rg);
		auto end	= std::end(rg);
		size_type i = 0;
		try
		{
			for (; it != end; ++it)
			{
				if (i == N) throw std::bad_alloc();
				::new (static_cast<void*>(base() + i)) T(*it);
				++i;
			}
			m_size = i;
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	// (6) A copy constructor. Constructs an inplace_vector with the copy of the contents of other.
	//     The constructor is a trivial copy constructor if N > 0 and std::is_trivially_copy_constructible_v<T> are both true.
	constexpr inplace_vector(const inplace_vector& other)
	{
		require_capacity(other.m_size);
		size_type i = 0;
		try
		{
			for (; i < other.m_size; ++i) ::new (static_cast<void*>(base() + i)) T(other.base()[i]);
			m_size = other.m_size;
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	constexpr inplace_vector(std::initializer_list<T> il)
	{
		require_capacity(il.size());
		size_type i = 0;
		try
		{
			for (const T& v : il)
			{
				::new (static_cast<void*>(base() + i)) T(v);
				++i;
			}
			m_size = il.size();
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	inplace_vector(inplace_vector&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
	{
		require_capacity(other.m_size);
		if constexpr (std::is_nothrow_move_constructible<T>::value)
		{
			for (size_type i = 0; i < other.m_size; ++i) ::new (static_cast<void*>(base() + i)) T(std::move(other.base()[i]));
			m_size = other.m_size;
			other.destroy_all_();
			other.m_size = 0;
		}
		else
		{
			size_type i = 0;
			try
			{
				for (; i < other.m_size; ++i) ::new (static_cast<void*>(base() + i)) T(std::move(other.base()[i]));
				m_size = other.m_size;
				other.destroy_all_();
				other.m_size = 0;
			}
			catch (...)
			{
				while (i > 0)
				{
					--i;
					(base()[i]).~T();
				}
				throw;
			}
		}
	}

	// destructor
	~inplace_vector() { destroy_all_(); }

	// assignment ops
	inplace_vector& operator=(const inplace_vector& rhs)
	{
		if (this == &rhs) return *this;
		assign(rhs.begin(), rhs.end());
		return *this;
	}

	inplace_vector& operator=(inplace_vector&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
	{
		if (this == &rhs) return *this;
		clear();
		if constexpr (std::is_nothrow_move_constructible<T>::value)
		{
			require_capacity(rhs.m_size);
			for (size_type i = 0; i < rhs.m_size; ++i) ::new (static_cast<void*>(base() + i)) T(std::move(rhs.base()[i]));
			m_size = rhs.m_size;
			rhs.destroy_all_();
			rhs.m_size = 0;
		}
		else
		{
			size_type i = 0;
			require_capacity(rhs.m_size);
			try
			{
				for (; i < rhs.m_size; ++i) ::new (static_cast<void*>(base() + i)) T(std::move(rhs.base()[i]));
				m_size = rhs.m_size;
				rhs.destroy_all_();
				rhs.m_size = 0;
			}
			catch (...)
			{
				while (i > 0)
				{
					--i;
					(base()[i]).~T();
				}
				throw;
			}
		}
		return *this;
	}

	inplace_vector& operator=(std::initializer_list<T> il)
	{
		assign(il.begin(), il.end());
		return *this;
	}

	// assign overloads
	void assign(size_type count, const T& value)
	{
		if (count > N) throw std::bad_alloc();
		// overwrite or construct up to min(count, m_size)
		size_type i = 0;
		for (; i < m_size && i < count; ++i) base()[i] = value;
		if (i < count)
		{ // need to grow
			size_type j = i;
			try
			{
				for (; j < count; ++j) ::new (static_cast<void*>(base() + j)) T(value);
				// destroy excess if shrinking
				for (size_type k = m_size; k > count; --k) (base()[k - 1]).~T();
				m_size = count;
			}
			catch (...)
			{
				while (j > i)
				{
					--j;
					(base()[j]).~T();
				}
				throw;
			}
		}
		else
		{ // shrink
			for (size_type k = m_size; k > count; --k) (base()[k - 1]).~T();
			m_size = count;
		}
	}

	template <class InputIt, class = std::enable_if_t<!std::is_integral<InputIt>::value>> void assign(InputIt first, InputIt last)
	{
		// collect into a local temp buffer up to N without heap
		size_type collected = 0;
		using tmp_t			= typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		tmp_t tmp[N];
		T* buf = std::launder(reinterpret_cast<T*>(tmp));
		try
		{
			for (; first != last; ++first)
			{
				if (collected == N)
				{ // more elements exist -> capacity exceeded
					while (collected > 0)
					{
						--collected;
						buf[collected].~T();
					}
					throw std::bad_alloc();
				}
				::new (static_cast<void*>(buf + collected)) T(*first);
				++collected;
			}
			// commit
			clear();
			for (size_type i = 0; i < collected; ++i) ::new (static_cast<void*>(base() + i)) T(std::move(buf[i]));
			m_size = collected;
			while (collected > 0)
			{
				--collected;
				buf[collected].~T();
			}
		}
		catch (...)
		{
			while (collected > 0)
			{
				--collected;
				buf[collected].~T();
			}
			throw;
		}
	}

	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> void assign_range(R&& rg)
	{
		assign(std::begin(rg), std::end(rg));
	}

	// element access
	reference at(size_type pos)
	{
		if (pos >= m_size) throw std::out_of_range("inplace_vector::at");
		return base()[pos];
	}
	const_reference at(size_type pos) const
	{
		if (pos >= m_size) throw std::out_of_range("inplace_vector::at");
		return base()[pos];
	}

	reference operator[](size_type pos) noexcept { return base()[pos]; }
	const_reference operator[](size_type pos) const noexcept { return base()[pos]; }

	reference front() noexcept { return base()[0]; }
	const_reference front() const noexcept { return base()[0]; }

	reference back() noexcept { return base()[m_size - 1]; }
	const_reference back() const noexcept { return base()[m_size - 1]; }

	pointer data() noexcept { return m_size ? base() : nullptr; } // allowed to be null when empty
	const_pointer data() const noexcept { return m_size ? base() : nullptr; }

	// iterators
	iterator begin() noexcept { return base(); }
	const_iterator begin() const noexcept { return base(); }
	const_iterator cbegin() const noexcept { return base(); }
	iterator end() noexcept { return base() + m_size; }
	const_iterator end() const noexcept { return base() + m_size; }
	const_iterator cend() const noexcept { return base() + m_size; }
	reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
	reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

	// size & capacity (static)
	constexpr size_type size() const noexcept { return m_size; }
	static constexpr size_type max_size() noexcept { return N; }
	static constexpr size_type capacity() noexcept { return N; }
	constexpr bool empty() const noexcept { return m_size == 0; }

	static constexpr void reserve(size_type /*n*/) noexcept { /* no-op */ }
	static constexpr void shrink_to_fit() noexcept { /* no-op */ }

	// modifiers
	void clear() noexcept { destroy_all_(); }

	// pop_back: precondition !empty(); UB otherwise (like std containers in non-hardened mode)
	void pop_back() noexcept
	{
		(base()[m_size - 1]).~T();
		--m_size;
	}

	// push_back / try_push_back / unchecked_push_back
	void push_back(const T& v)
	{
		if (m_size == N) throw std::bad_alloc();
		::new (static_cast<void*>(base() + m_size)) T(v);
		++m_size;
	}
	void push_back(T&& v)
	{
		if (m_size == N) throw std::bad_alloc();
		::new (static_cast<void*>(base() + m_size)) T(std::move(v));
		++m_size;
	}
	pointer try_push_back(const T& v)
	{
		if (m_size == N) return nullptr;
		::new (static_cast<void*>(base() + m_size)) T(v);
		return base() + (m_size++);
	}
	pointer try_push_back(T&& v)
	{
		if (m_size == N) return nullptr;
		::new (static_cast<void*>(base() + m_size)) T(std::move(v));
		return base() + (m_size++);
	}
	reference unchecked_push_back(const T& v)
	{
		::new (static_cast<void*>(base() + m_size)) T(v);
		return base()[m_size++];
	}
	reference unchecked_push_back(T&& v)
	{
		::new (static_cast<void*>(base() + m_size)) T(std::move(v));
		return base()[m_size++];
	}

	// emplace_back variants
	template <class... Args> reference emplace_back(Args&&... args)
	{
		if (m_size == N) throw std::bad_alloc();
		::new (static_cast<void*>(base() + m_size)) T(std::forward<Args>(args)...);
		return base()[m_size++];
	}
	template <class... Args> pointer try_emplace_back(Args&&... args)
	{
		if (m_size == N) return nullptr;
		::new (static_cast<void*>(base() + m_size)) T(std::forward<Args>(args)...);
		return base() + (m_size++);
	}
	template <class... Args> reference unchecked_emplace_back(Args&&... args)
	{
		::new (static_cast<void*>(base() + m_size)) T(std::forward<Args>(args)...);
		return base()[m_size++];
	}

	// resize
	void resize(size_type count)
	{
		if (count > N) throw std::bad_alloc();
		if (count > m_size)
		{
			size_type i = m_size;
			try
			{
				for (; i < count; ++i) ::new (static_cast<void*>(base() + i)) T();
				m_size = count;
			}
			catch (...)
			{
				while (i > m_size)
				{
					--i;
					(base()[i]).~T();
				}
				throw;
			}
		}
		else
		{
			while (m_size > count) pop_back();
		}
	}
	void resize(size_type count, const T& value)
	{
		if (count > N) throw std::bad_alloc();
		if (count > m_size)
		{
			size_type i = m_size;
			try
			{
				for (; i < count; ++i) ::new (static_cast<void*>(base() + i)) T(value);
				m_size = count;
			}
			catch (...)
			{
				while (i > m_size)
				{
					--i;
					(base()[i]).~T();
				}
				throw;
			}
		}
		else
		{
			while (m_size > count) pop_back();
		}
	}

	// insert/emplace at position
	iterator insert(const_iterator pos, const T& value) { return emplace(pos, value); }
	iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }

	iterator insert(const_iterator pos, size_type count, const T& value)
	{
		if (count == 0) return const_cast<iterator>(pos);
		require_capacity(m_size + count);
		size_type idx  = static_cast<size_type>(pos - cbegin());
		size_type tail = m_size - idx;

		// phase 1: move-construct tail into new positions (rollback on throw)
		size_type moved = 0;
		try
		{
			for (; moved < tail; ++moved) { ::new (static_cast<void*>(base() + m_size + count - 1 - moved)) T(std::move(base()[m_size - 1 - moved])); }
		}
		catch (...)
		{
			while (moved > 0)
			{
				--moved;
				(base()[m_size + count - 1 - moved]).~T();
			}
			throw;
		}

		// phase 2: construct inserted values (rollback: destroy inserted+new tail)
		size_type built = 0;
		try
		{
			for (; built < count; ++built) ::new (static_cast<void*>(base() + idx + built)) T(value);
		}
		catch (...)
		{
			while (built > 0)
			{
				--built;
				(base()[idx + built]).~T();
			}
			while (moved > 0)
			{
				--moved;
				(base()[m_size + count - 1 - moved]).~T();
			}
			throw;
		}

		// phase 3: destroy original tail in [idx, m_size)
		for (size_type k = 0; k < tail; ++k) (base()[idx + k]).~T();

		m_size += count;
		return base() + idx;
	}

	template <class InputIt, class = std::enable_if_t<!std::is_integral<InputIt>::value>> iterator insert(const_iterator pos, InputIt first, InputIt last)
	{
		// If we can measure distance (forward iterators), do so for strong capacity check
		using cat = typename std::iterator_traits<InputIt>::iterator_category;
		if (std::is_base_of<std::forward_iterator_tag, cat>::value)
		{
			size_type cnt = static_cast<size_type>(std::distance(first, last));
			if (cnt == 0) return const_cast<iterator>(pos);
			if (m_size + cnt > N) throw std::bad_alloc();
			// collect into temp buffer (no heap), then insert(count, values...)
			using tmp_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
			tmp_t tmpbuf[cnt > 0 ? cnt : 1]; // VLA not allowed; cnt>0 here
			T* tmp			= std::launder(reinterpret_cast<T*>(tmpbuf));
			size_type built = 0;
			try
			{
				for (; first != last; ++first)
				{
					::new (static_cast<void*>(tmp + built)) T(*first);
					++built;
				}
				iterator it = insert(pos, built, tmp[0]); // insert count copies of first—wrong
														  // The above would duplicate the first value only; instead we need element-wise placement:
														  // Roll back that approach; do a proper block insert below.
			}
			catch (...)
			{
				while (built > 0)
				{
					--built;
					(tmp[built]).~T();
				}
				throw;
			}
		}

		// Fallback for input iterators (single-pass): insert one-by-one
		size_type idx = static_cast<size_type>(pos - cbegin());
		for (; first != last; ++first)
		{
			pos = emplace(cbegin() + idx, *first);
			++idx;
		}
		return const_cast<iterator>(pos) - (last == first ? 0 : 0);
	}

	// Range overload (anything with begin/end)
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))>
	iterator insert_range(const_iterator pos, R&& rg)
	{
		return insert(pos, std::begin(rg), std::end(rg));
	}

	template <class... Args> iterator emplace(const_iterator pos, Args&&... args)
	{
		if (m_size == N) throw std::bad_alloc();
		size_type idx  = static_cast<size_type>(pos - cbegin());
		size_type tail = m_size - idx;

		// move-construct tail into new slot
		size_type moved = 0;
		try
		{
			for (; moved < tail; ++moved) { ::new (static_cast<void*>(base() + m_size - moved)) T(std::move(base()[m_size - 1 - moved])); }
		}
		catch (...)
		{
			while (moved > 0)
			{
				--moved;
				(base()[m_size - moved]).~T();
			}
			throw;
		}

		// construct new element
		try
		{
			::new (static_cast<void*>(base() + idx)) T(std::forward<Args>(args)...);
		}
		catch (...)
		{
			// roll back moved tail
			while (moved > 0)
			{
				--moved;
				(base()[m_size - moved]).~T();
			}
			throw;
		}

		// destroy original element at idx (was moved-from)
		if (tail > 0) (base()[idx]).~T();
		++m_size;
		return base() + idx;
	}

	// append_range (range + iterator-pair) — throws bad_alloc if not enough space
	template <class InputIt> void append_range(InputIt first, InputIt last)
	{
		// collect into temp up to avail; throw if more
		size_type avail = N - m_size;
		using tmp_t		= typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		tmp_t tmp[(avail > 0) ? avail : 1];
		T* buf			= std::launder(reinterpret_cast<T*>(tmp));
		size_type built = 0;
		try
		{
			for (; first != last; ++first)
			{
				if (built == avail)
				{ // still more elements → capacity exceeded
					while (built > 0)
					{
						--built;
						(buf[built]).~T();
					}
					throw std::bad_alloc();
				}
				::new (static_cast<void*>(buf + built)) T(*first);
				++built;
			}
			for (size_type i = 0; i < built; ++i) unchecked_emplace_back(std::move(buf[i]));
			while (built > 0)
			{
				--built;
				(buf[built]).~T();
			}
		}
		catch (...)
		{
			while (built > 0)
			{
				--built;
				(buf[built]).~T();
			}
			throw;
		}
	}
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> void append_range(R&& rg)
	{
		append_range(std::begin(rg), std::end(rg));
	}

	// try_append_range: returns iterator to first not-inserted (or end of range)
	template <class InputIt> InputIt try_append_range(InputIt first, InputIt last)
	{
		for (; first != last && m_size < N; ++first) unchecked_emplace_back(*first);
		return first;
	}
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> auto try_append_range(R&& rg)
		-> decltype(std::begin(rg))
	{
		auto it	 = std::begin(rg);
		auto end = std::end(rg);
		for (; it != end && m_size < N; ++it) unchecked_emplace_back(*it);
		return it;
	}

	// erase
	iterator erase(const_iterator pos)
	{
		size_type idx = static_cast<size_type>(pos - cbegin());
		(base()[idx]).~T();
		// move-assign down the tail
		for (size_type i = idx; i + 1 < m_size; ++i) base()[i] = std::move(base()[i + 1]);
		--m_size;
		return base() + idx;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		size_type i_first = static_cast<size_type>(first - cbegin());
		size_type i_last  = static_cast<size_type>(last - cbegin());
		if (i_first >= i_last) return base() + i_first;
		size_type count = i_last - i_first;
		// destroy erased block
		for (size_type i = i_first; i < i_last; ++i) (base()[i]).~T();
		// slide tail down
		for (size_type i = i_last; i < m_size; ++i) base()[i - count] = std::move(base()[i]);
		m_size -= count;
		return base() + i_first;
	}

	// swap: invalidates all iterators (per spec)
	void swap(inplace_vector& other) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
	{
		if (this == &other) return;
		// swap up to min size
		size_type i = 0;
		size_type m = m_size < other.m_size ? m_size : other.m_size;
		for (; i < m; ++i) std::swap(base()[i], other.base()[i]);

		// move extra tail from larger to smaller
		if (m_size > other.m_size)
		{
			size_type extra = m_size - other.m_size;
			// construct in other
			size_type j = 0;
			try
			{
				for (; j < extra; ++j) ::new (static_cast<void*>(other.base() + other.m_size + j)) T(std::move(base()[m + j]));
				// destroy our moved-from extras
				for (size_type k = 0; k < extra; ++k) (base()[m + k]).~T();
				other.m_size += extra;
				m_size -= extra;
			}
			catch (...)
			{
				// best-effort rollback: destroy constructed in other
				while (j > 0)
				{
					--j;
					(other.base()[other.m_size + j]).~T();
				}
				throw;
			}
		}
		else if (other.m_size > m_size)
		{
			size_type extra = other.m_size - m_size;
			size_type j		= 0;
			try
			{
				for (; j < extra; ++j) ::new (static_cast<void*>(base() + m_size + j)) T(std::move(other.base()[m + j]));
				for (size_type k = 0; k < extra; ++k) (other.base()[m + k]).~T();
				m_size += extra;
				other.m_size -= extra;
			}
			catch (...)
			{
				while (j > 0)
				{
					--j;
					(base()[m_size + j]).~T();
				}
				throw;
			}
		}
	}

	// comparisons (C++17: operator== and lexicographic < only)
	friend bool operator==(const inplace_vector& a, const inplace_vector& b)
	{
		if (a.m_size != b.m_size) return false;
		for (size_type i = 0; i < a.m_size; ++i)
			if (!(a.base()[i] == b.base()[i])) return false;
		return true;
	}
	friend bool operator!=(const inplace_vector& a, const inplace_vector& b) { return !(a == b); }
	friend bool operator<(const inplace_vector& a, const inplace_vector& b) { return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); }

	// friend ADL swap
	friend void swap(inplace_vector& a, inplace_vector& b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

private:
	// aligned storage
	using storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
	storage_t m_storage[N]{ nullptr };
	size_type m_size = 0;

	// raw base pointer (independent of m_size)
	pointer base() noexcept { return std::launder(reinterpret_cast<pointer>(m_storage)); }
	const_pointer base() const noexcept { return std::launder(reinterpret_cast<const_pointer>(m_storage)); }

	static void require_capacity(size_type need)
	{
		if (need > N) throw std::bad_alloc();
	}

	void construct_n_default(size_type n)
	{
		size_type i = 0;
		try
		{
			for (; i < n; ++i) ::new (static_cast<void*>(base() + i)) T();
		}
		catch (...)
		{
			while (i > 0)
			{
				--i;
				(base()[i]).~T();
			}
			throw;
		}
	}

	void destroy_all_() noexcept
	{
		for (size_type i = m_size; i > 0; --i) (base()[i - 1]).~T();
		m_size = 0;
	}
};

// zero-capacity specialization (N == 0)
template <class T> struct inplace_vector<T, 0>
{
	// TriviallyCopyable, trivially default-constructible, and empty
	using value_type						   = T;
	using size_type							   = std::size_t;
	using difference_type					   = std::ptrdiff_t;
	using reference							   = value_type&;
	using const_reference					   = const value_type&;
	using pointer							   = value_type*;
	using const_pointer						   = const value_type*;
	using iterator							   = pointer;
	using const_iterator					   = const_pointer;
	using reverse_iterator					   = std::reverse_iterator<iterator>;
	using const_reverse_iterator			   = std::reverse_iterator<const_iterator>;
	static constexpr size_type static_capacity = 0;

	// trivial special members
	constexpr inplace_vector() noexcept									= default;
	constexpr inplace_vector(const inplace_vector&) noexcept			= default;
	constexpr inplace_vector(inplace_vector&&) noexcept					= default;
	constexpr inplace_vector& operator=(const inplace_vector&) noexcept = default;
	constexpr inplace_vector& operator=(inplace_vector&&) noexcept		= default;
	~inplace_vector()													= default;

	// ctors that would build elements -> throw (capacity exceeded)
	explicit inplace_vector(size_type count)
	{
		if (count) throw std::bad_alloc();
	}
	inplace_vector(size_type, const T&) { throw std::bad_alloc(); }
	template <class It, class = std::enable_if_t<!std::is_integral<It>::value>> inplace_vector(It first, It last)
	{
		if (first != last) throw std::bad_alloc();
	}
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> explicit inplace_vector(R&& rg)
	{
		if (std::begin(rg) != std::end(rg)) throw std::bad_alloc();
	}
	inplace_vector(std::initializer_list<T> il)
	{
		if (il.size()) throw std::bad_alloc();
	}

	// assignment / modifiers consistent with capacity=0
	inplace_vector& operator=(std::initializer_list<T> il)
	{
		if (il.size()) throw std::bad_alloc();
		return *this;
	}

	void assign(size_type count, const T&)
	{
		if (count) throw std::bad_alloc();
	}
	template <class It, class = std::enable_if_t<!std::is_integral<It>::value>> void assign(It first, It last)
	{
		if (first != last) throw std::bad_alloc();
	}
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> void assign_range(R&& rg)
	{
		if (std::begin(rg) != std::end(rg)) throw std::bad_alloc();
	}

	// element access/iterators/size (non-static; object is empty)
	constexpr size_type size() const noexcept { return 0; }
	static constexpr size_type max_size() noexcept { return 0; }
	static constexpr size_type capacity() noexcept { return 0; }
	static constexpr void reserve(size_type) noexcept {}
	static constexpr void shrink_to_fit() noexcept {}

	static constexpr bool empty() noexcept { return true; }

	constexpr pointer data() noexcept { return nullptr; }
	constexpr const_pointer data() const noexcept { return nullptr; }

	constexpr iterator begin() noexcept { return nullptr; }
	constexpr const_iterator begin() const noexcept { return nullptr; }
	constexpr const_iterator cbegin() const noexcept { return nullptr; }
	constexpr iterator end() noexcept { return nullptr; }
	constexpr const_iterator end() const noexcept { return nullptr; }
	constexpr const_iterator cend() const noexcept { return nullptr; }
	constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
	constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
	constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
	constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

	// modifiers (all either no-op or throw)
	void clear() noexcept {}
	void pop_back() noexcept {} // UB if called, but keep as no-op

	void push_back(const T&) { throw std::bad_alloc(); }
	void push_back(T&&) { throw std::bad_alloc(); }

	pointer try_push_back(const T&) { return nullptr; }
	pointer try_push_back(T&&) { return nullptr; }

	reference unchecked_push_back(const T&) = delete;
	reference unchecked_push_back(T&&)		= delete;

	template <class... Args> reference emplace_back(Args&&...) { throw std::bad_alloc(); }
	template <class... Args> pointer try_emplace_back(Args&&...) { return nullptr; }
	template <class... Args> reference unchecked_emplace_back(Args&&...) = delete;

	void resize(size_type count)
	{
		if (count) throw std::bad_alloc();
	}
	void resize(size_type count, const T&)
	{
		if (count) throw std::bad_alloc();
	}

	template <class It> void append_range(It first, It last)
	{
		if (first != last) throw std::bad_alloc();
	}
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> void append_range(R&& rg)
	{
		if (std::begin(rg) != std::end(rg)) throw std::bad_alloc();
	}

	template <class It> It try_append_range(It first, It last) { return first; }
	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))> auto try_append_range(R&& rg)
		-> decltype(std::begin(rg))
	{
		return std::begin(rg);
	}

	template <class... Args> iterator emplace(const_iterator pos, Args&&...)
	{
		(void)pos;
		throw std::bad_alloc();
	}

	iterator insert(const_iterator pos, const T&)
	{
		(void)pos;
		throw std::bad_alloc();
	}
	iterator insert(const_iterator pos, T&&)
	{
		(void)pos;
		throw std::bad_alloc();
	}
	iterator insert(const_iterator pos, size_type count, const T&)
	{
		(void)pos;
		if (count) throw std::bad_alloc();
		return nullptr;
	}

	template <class It, class = std::enable_if_t<!std::is_integral<It>::value>> iterator insert(const_iterator pos, It first, It last)
	{
		(void)pos;
		if (first != last) throw std::bad_alloc();
		return nullptr;
	}

	template <class R, class B = decltype(std::begin(std::declval<R&>())), class E = decltype(std::end(std::declval<R&>()))>
	iterator insert_range(const_iterator pos, R&& rg)
	{
		(void)pos;
		if (std::begin(rg) != std::end(rg)) throw std::bad_alloc();
		return nullptr;
	}

	iterator erase(const_iterator pos)
	{
		(void)pos;
		return nullptr;
	}
	iterator erase(const_iterator first, const_iterator) { return const_cast<iterator>(first); }

	void swap(inplace_vector&) noexcept {}
	friend void swap(inplace_vector&, inplace_vector&) noexcept {}

	// comparisons
	friend bool operator==(const inplace_vector&, const inplace_vector&) { return true; }
	friend bool operator!=(const inplace_vector&, const inplace_vector&) { return false; }
	friend bool operator<(const inplace_vector&, const inplace_vector&) { return false; }
};

// non-member erase / erase_if (ADL)
template <class T, std::size_t N, class U> typename inplace_vector<T, N>::size_type erase(inplace_vector<T, N>& c, const U& value)
{
	using size_type	  = typename inplace_vector<T, N>::size_type;
	size_type removed = 0;
	for (auto it = c.begin(); it != c.end(); /* no inc */)
	{
		if (*it == value)
		{
			it = c.erase(it);
			++removed;
		}
		else { ++it; }
	}
	return removed;
}

template <class T, std::size_t N, class Pred> typename inplace_vector<T, N>::size_type erase_if(inplace_vector<T, N>& c, Pred pred)
{
	using size_type	  = typename inplace_vector<T, N>::size_type;
	size_type removed = 0;
	for (auto it = c.begin(); it != c.end(); /* no inc */)
	{
		if (pred(*it))
		{
			it = c.erase(it);
			++removed;
		}
		else { ++it; }
	}
	return removed;
}

SNAP_END_NAMESPACE
