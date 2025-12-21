#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE
template <class T> struct remove_cvref
	{
		using type = std::remove_cv_t<std::remove_reference_t<T>>;
	};

	template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
SNAP_END_NAMESPACE
