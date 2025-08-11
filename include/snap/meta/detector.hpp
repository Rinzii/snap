#pragma once

#include "snap/type_traits/type_identity.hpp"

namespace snap::meta
{

	template<class T, class Void, template<class...> class, class...>
	struct detector : type_identity<T>
	{
		using value_t = std::false_type;
	};

	template<class T, template<class...> class U, class... Args>
	struct detector<T, std::void_t<U<Args...>>, U, Args...> : type_identity<U<Args...>>
	{
		using value_t = std::true_type;
	};

	struct nonesuch final
	{
		nonesuch(nonesuch const &) = delete;
		nonesuch() = delete;
		~nonesuch() = delete;
		void operator=(nonesuch const &) = delete;
	};

	template<class T, template<class...> class U, class... Args>
	using detected_or = detector<T, void, U, Args...>;

	template<template<class...> class T, class... Args>
	using detected_t = typename detected_or<nonesuch, T, Args...>::type;

	template<class T, template<class...> class U, class... Args>
	using detected_or_t = typename detected_or<T, U, Args...>::type;

	template<template<class...> class T, class... Args>
	using is_detected = typename detected_or<nonesuch, T, Args...>::value_t;

	template<class To, template<class...> class T, class... Args>
	using is_detected_convertible = std::is_convertible<
		detected_t<T, Args...>,
		To>;

	template<class T, template<class...> class U, class... Args>
	using is_detected_exact = std::is_same<T, detected_t<U, Args...>>;
} // namespace snap::meta
