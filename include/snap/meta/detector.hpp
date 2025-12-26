#ifndef SNP_INCLUDE_SNAP_META_DETECTOR_HPP
#define SNP_INCLUDE_SNAP_META_DETECTOR_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/type_traits/type_identity.hpp"

#include <type_traits>

SNAP_BEGIN_NAMESPACE

/**
 * \brief Core detection template.
 *
 * Primary template is selected when `Probe<Args...>` is not valid.
 *
 * \tparam FallbackType Type used when detection fails.
 * \tparam SfinaeTag    SFINAE helper parameter.
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, class SfinaeTag, template <class...> class Probe, class... Args> struct detector : ::SNAP_NAMESPACE::type_identity<FallbackType>
{
	/**
	 * \brief `std::true_type` on success, otherwise `std::false_type`.
	 */
	using value_t = std::false_type;
};

/**
 * \brief Core detection specialization for the valid case.
 *
 * Selected when `Probe<Args...>` is valid.
 *
 * \tparam FallbackType Type used when detection fails (unused on success).
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, template <class...> class Probe, class... Args> struct detector<FallbackType, std::void_t<Probe<Args...>>, Probe, Args...>
	: ::SNAP_NAMESPACE::type_identity<Probe<Args...>>
{
	/**
	 * \brief `std::true_type` on success, otherwise `std::false_type`.
	 */
	using value_t = std::true_type;
};

/**
 * \brief Sentinel type used as the default fallback.
 *
 * This type is intentionally not constructible, assignable, or allocatable.
 */
struct nonesuch final
{
	nonesuch()							 = delete;
	~nonesuch()							 = delete;
	nonesuch(nonesuch const&)			 = delete;
	nonesuch(nonesuch&&)				 = delete;
	nonesuch& operator=(nonesuch const&) = delete;
	nonesuch& operator=(nonesuch&&)		 = delete;

	void* operator new(std::size_t)	  = delete;
	void* operator new[](std::size_t) = delete;
	void operator delete(void*)		  = delete;
	void operator delete[](void*)	  = delete;
};

/**
 * \brief `detector` instantiation that yields either a detected type or a fallback.
 *
 * \tparam FallbackType Type used when detection fails.
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, template <class...> class Probe, class... Args> using detected_or = detector<FallbackType, void, Probe, Args...>;

/**
 * \brief Detected type if valid, otherwise `nonesuch`.
 *
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <template <class...> class Probe, class... Args> using detected_t = typename detected_or<nonesuch, Probe, Args...>::type;

/**
 * \brief Detected type if valid, otherwise the provided fallback.
 *
 * \tparam FallbackType Type used when detection fails.
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, template <class...> class Probe, class... Args> using detected_or_t = typename detected_or<FallbackType, Probe, Args...>::type;

/**
 * \brief `std::true_type` if `Probe<Args...>` is valid, otherwise `std::false_type`.
 *
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <template <class...> class Probe, class... Args> using is_detected = typename detected_or<nonesuch, Probe, Args...>::value_t;

/**
 * \brief `bool` constant for `is_detected`.
 *
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <template <class...> class Probe, class... Args> inline constexpr bool is_detected_v = is_detected<Probe, Args...>::value;

/**
 * \brief Alias for `is_detected<Probe, Args...>`.
 *
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <template <class...> class Probe, class... Args> using is_detected_t = is_detected<Probe, Args...>;

/**
 * \brief Checks whether the detected type is convertible to `To`.
 *
 * If detection fails, the result is `false`.
 *
 * \tparam To    Target type.
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <class To, template <class...> class Probe, class... Args> using is_detected_convertible = std::is_convertible<detected_t<Probe, Args...>, To>;

/**
 * \brief `bool` constant for `is_detected_convertible`.
 *
 * \tparam To    Target type.
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <class To, template <class...> class Probe, class... Args> inline constexpr bool is_detected_convertible_v =
	is_detected_convertible<To, Probe, Args...>::value;

/**
 * \brief Alias for `is_detected_convertible<To, Probe, Args...>`.
 *
 * \tparam To    Target type.
 * \tparam Probe Template to probe.
 * \tparam Args  Arguments for `Probe`.
 */
template <class To, template <class...> class Probe, class... Args> using is_detected_convertible_t = is_detected_convertible<To, Probe, Args...>;

/**
 * \brief Checks whether the detected type is exactly `Expected`.
 *
 * If detection fails, the result is `false` unless `Expected` is `nonesuch`.
 *
 * \tparam Expected Expected detected type.
 * \tparam Probe    Template to probe.
 * \tparam Args     Arguments for `Probe`.
 */
template <class Expected, template <class...> class Probe, class... Args> using is_detected_exact = std::is_same<Expected, detected_t<Probe, Args...>>;

/**
 * \brief `bool` constant for `is_detected_exact`.
 *
 * \tparam Expected Expected detected type.
 * \tparam Probe    Template to probe.
 * \tparam Args     Arguments for `Probe`.
 */
template <class Expected, template <class...> class Probe, class... Args> inline constexpr bool is_detected_exact_v =
	is_detected_exact<Expected, Probe, Args...>::value;

/**
 * \brief Alias for `is_detected_exact<Expected, Probe, Args...>`.
 *
 * \tparam Expected Expected detected type.
 * \tparam Probe    Template to probe.
 * \tparam Args     Arguments for `Probe`.
 */
template <class Expected, template <class...> class Probe, class... Args> using is_detected_exact_t = is_detected_exact<Expected, Probe, Args...>;

/**
 * \brief Like `is_detected`, with an explicit fallback type parameter.
 *
 * Provided mainly for symmetry with `detected_or`.
 *
 * \tparam FallbackType Type used when detection fails.
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, template <class...> class Probe, class... Args> using is_detected_or =
	typename detected_or<FallbackType, Probe, Args...>::value_t;

/**
 * \brief `bool` constant for `is_detected_or`.
 *
 * \tparam FallbackType Type used when detection fails.
 * \tparam Probe        Template to probe.
 * \tparam Args         Arguments for `Probe`.
 */
template <class FallbackType, template <class...> class Probe, class... Args> inline constexpr bool is_detected_or_v =
	is_detected_or<FallbackType, Probe, Args...>::value;

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_META_DETECTOR_HPP
