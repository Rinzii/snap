#pragma once

#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/type_identity.hpp"

SNAP_BEGIN_NAMESPACE
// Primary: expression is ill-formed -> yields fallback type T and false
	template <class T, class, template <class...> class, class...> struct detector : type_identity<T>
	{
		using value_t = std::false_type;
	};

	// Specialization: expression well-formed -> yields U<Args...> and true
	template <class T, template <class...> class U, class... Args> struct detector<T, std::void_t<U<Args...>>, U, Args...> : type_identity<U<Args...>>
	{
		using value_t = std::true_type;
	};

	// Sentinel used when detection fails
	struct nonesuch final
	{
		nonesuch()							 = delete;
		~nonesuch()							 = delete;
		nonesuch(nonesuch const&)			 = delete;
		nonesuch(nonesuch&&)				 = delete;
		nonesuch& operator=(nonesuch const&) = delete;
		nonesuch& operator=(nonesuch&&)		 = delete;
	};

	// detected_or: returns detector with either detected type or fallback T
	template <class T, template <class...> class U, class... Args> using detected_or = detector<T, void, U, Args...>;

	// detected_t: detected type if well-formed, otherwise nonesuch
	template <template <class...> class T, class... Args> using detected_t = typename detected_or<nonesuch, T, Args...>::type;

	// detected_or_t: detected type if well-formed, otherwise provided fallback T
	template <class T, template <class...> class U, class... Args> using detected_or_t = typename detected_or<T, U, Args...>::type;

	// is_detected: std::true_type if well-formed, else std::false_type
	template <template <class...> class T, class... Args> using is_detected = typename detected_or<nonesuch, T, Args...>::value_t;

	template <template <class...> class T, class... Args> inline constexpr bool is_detected_v = is_detected<T, Args...>::value;

	// is_detected_convertible: result convertible to To (false on failure)
	template <class To, template <class...> class T, class... Args> using is_detected_convertible = std::is_convertible<detected_t<T, Args...>, To>;

	template <class To, template <class...> class T, class... Args> inline constexpr bool is_detected_convertible_v =
		is_detected_convertible<To, T, Args...>::value;

	// is_detected_exact: result exactly T (false on failure)
	template <class T, template <class...> class U, class... Args> using is_detected_exact = std::is_same<T, detected_t<U, Args...>>;

	template <class T, template <class...> class U, class... Args> inline constexpr bool is_detected_exact_v = is_detected_exact<T, U, Args...>::value;
SNAP_END_NAMESPACE
