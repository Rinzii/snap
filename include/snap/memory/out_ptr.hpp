#pragma once

#include "snap/internal/abi_namespace.hpp"
#include "snap/memory/pointer_of.hpp"
#include "snap/memory/ptr_traits.hpp"

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

SNAP_BEGIN_NAMESPACE
template <class Smart, class Pointer, class... Args> class out_ptr_t
{
	static_assert(!snap::is_specialization_of_v<Smart, std::shared_ptr> || sizeof...(Args) > 0,
				  "Using std::shared_ptr<> without a deleter in snap::out_ptr is not supported.");

public:
	explicit out_ptr_t(Smart& smart, Args... args) noexcept : m_smart(smart), m_args(std::forward<Args>(args)...), m_ptr()
	{
		using Ptr = decltype(smart);
		if constexpr (is_resettable_smart_pointer_v<Ptr>) { m_smart.reset(); }
		else if constexpr (std::is_constructible_v<Smart>) { m_smart = Smart(); }
		else
		{
			static_assert(is_resettable_smart_pointer_v<Ptr> || std::is_constructible_v<Smart>,
						  "The adapted pointer type must have a reset() member function or be default constructible.");
		}
	}

	out_ptr_t(const out_ptr_t& /*unused*/) = delete;

	~out_ptr_t()
	{
		if (!m_ptr) { return; }

		using SmartPtr = pointer_of_or_t<Smart, Pointer>;
		if constexpr (is_resettable_smart_pointer_with_args_v<Smart, Pointer, Args...>)
		{
			std::apply([&](auto&&... m_args) { m_smart.reset(static_cast<SmartPtr>(m_ptr), std::forward<Args>(m_args)...); }, std::move(m_args));
		}
		else
		{
			static_assert(std::is_constructible_v<Smart, SmartPtr, Args...>,
						  "The smart pointer must be constructible from arguments of types _Smart, _Pointer, _Args...");

			std::apply([&](auto&&... m_args) { m_smart = Smart(static_cast<SmartPtr>(m_ptr), std::forward<Args>(m_args)...); }, std::move(m_args));
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

template <class Pointer = void, class Smart, class... Args> auto out_ptr(Smart& smart, Args&&... args)
{
	using Ptr = std::conditional_t<std::is_void_v<Pointer>, pointer_of_t<Smart>, Pointer>;
	return out_ptr_t<Smart, Ptr, Args&&...>(smart, std::forward<Args>(args)...);
}

SNAP_END_NAMESPACE
