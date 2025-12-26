#ifndef SNP_TESTS_UNIT_MEMORY_INOUT_PTR_HPP
#define SNP_TESTS_UNIT_MEMORY_INOUT_PTR_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/memory/pointer_of.hpp"
#include "snap/memory/ptr_traits.hpp"

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
template <class Smart, class Pointer, class... Args> class inout_ptr_t
{
	static_assert(!is_specialization_of_v<Smart, std::shared_ptr>, "Using std::shared_ptr<> without a deleter in SNAP_NAMESPACE::inout_ptr is not supported.");

public:
	explicit inout_ptr_t(Smart& smart, Args&&... args)
		: m_smart(smart),
		  m_args(std::forward<Args>(args)...),
		  m_ptr(
			  [&smart]
			  {
				  if constexpr (std::is_pointer_v<Smart>) { return smart; }
				  else { return smart.get(); }
			  }())
	{
		if constexpr (is_releasable_smart_pointer_v<Smart>) { m_smart.release(); }
		else { m_smart = Smart(); }
	}

	inout_ptr_t(const inout_ptr_t&) = delete;

	~inout_ptr_t()
	{
		// LWG-3897: inout_ptr will not update raw pointer to null
		if constexpr (!std::is_pointer_v<Smart>)
		{
			if (!m_ptr) { return; }
		}

		using SmartPtr = SNAP_NAMESPACE::pointer_of_or_t<Smart, Pointer>;

		if constexpr (std::is_pointer_v<Smart>)
		{
			// Raw-pointer target: just store the produced pointer
			m_smart = static_cast<SmartPtr>(m_ptr);
		}
		else if constexpr (SNAP_NAMESPACE::is_resettable_smart_pointer_with_args_v<Smart, Pointer, Args...>)
		{
			// Smart pointer supports reset(ptr, args...)
			std::apply([&](auto&&... a) { m_smart.reset(static_cast<SmartPtr>(m_ptr), std::forward<decltype(a)>(a)...); }, std::move(m_args));
		}
		else
		{
			// Fallback: Smart constructible from (ptr, args...)
			static_assert(std::is_constructible_v<Smart, SmartPtr, Args...>,
						  "Smart must be constructible from (SmartPtr, Args...) "
						  "or provide reset(pointer, args...).");
			std::apply([&](auto&&... a) { m_smart = Smart(static_cast<SmartPtr>(m_ptr), std::forward<decltype(a)>(a)...); }, std::move(m_args));
		}
	}

	operator Pointer*() const noexcept { return std::addressof(const_cast<Pointer&>(m_ptr)); }

	template <class P = Pointer, std::enable_if_t<!std::is_same_v<P, void*>, int> = 0> operator void**() const noexcept
	{
		static_assert(std::is_pointer_v<Pointer>, "Conversion to void** requires Pointer to be a raw pointer.");
		return reinterpret_cast<void**>(static_cast<Pointer*>(*this));
	}

private:
	Smart& m_smart;
	std::tuple<Args...> m_args;
	Pointer m_ptr = Pointer();
};

template <class Pointer = void, class Smart, class... Args> auto inout_ptr(Smart& smart, Args&&... args)
{
	using Ptr = std::conditional_t<std::is_void_v<Pointer>, pointer_of_t<Smart>, Pointer>;
	return inout_ptr_t<Smart, Ptr, Args&&...>(smart, std::forward<Args>(args)...);
}

SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_MEMORY_INOUT_PTR_HPP
