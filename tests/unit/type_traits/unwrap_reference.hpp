#pragma once

#include <functional>
#include <type_traits>

namespace snap
{
	template <class T> struct unwrap_reference
	{
		using type = T;
	};
	template <class U> struct unwrap_reference<std::reference_wrapper<U>>
	{
		using type = U &;
	};

	template <class T> struct unwrap_ref_decay : unwrap_reference<std::decay_t<T>>
	{
	};

	template <class T> using unwrap_reference_t = typename unwrap_reference<T>::type;

	template <class T> using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
} // namespace snap
