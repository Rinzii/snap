#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <memory>  // std::allocator_traits
#include <utility> // std::forward

/**
 * @file
 * @brief RAII helper that constructs and destroys an object using an allocator.
 *
 * @details
 * The class manages the lifetime of a single object of type T without performing
 * any dynamic allocation. Construction and destruction are delegated to
 * std::allocator_traits for the provided allocator type.
 */

SNAP_BEGIN_NAMESPACE
/**
 * @brief Temporary value whose lifetime is managed via allocator traits.
 *
 * @tparam T Element type to construct and destroy.
 * @tparam Alloc Allocator type used with std::allocator_traits.
 *
 * @details
 * The object of type T is stored in a union to provide properly aligned raw
 * storage. It is constructed in place by allocator_traits::construct during
 * construction of temp_value and destroyed by allocator_traits::destroy in
 * the destructor. This is useful in container internals and other places where
 * allocator-aware construction and exception safety are required.
 */
template <class T, class Alloc> class temp_value
{
	using traits = std::allocator_traits<Alloc>;

public:
	/**
	 * @brief Copy construction is disabled.
	 */
	temp_value(const temp_value&) = delete;

	/**
	 * @brief Copy assignment is disabled.
	 */
	temp_value& operator=(const temp_value&) = delete;

	/**
	 * @brief Move construction is disabled.
	 */
	temp_value(temp_value&&) = delete;

	/**
	 * @brief Move assignment is disabled.
	 */
	temp_value& operator=(temp_value&&) = delete;

	/**
	 * @brief Constructs the contained object via allocator traits.
	 *
	 * @param alloc Allocator instance used for construction and destruction.
	 * @param args Arguments forwarded to the constructor of T.
	 *
	 * @post The contained T object is constructed and accessible via get.
	 * @remarks If construction throws, no object is created and the exception propagates.
	 */
	template <class... Args> explicit temp_value(Alloc& alloc, Args&&... args) : alloc_(alloc)
	{
		traits::construct(alloc_, addr(), std::forward<Args>(args)...);
	}

	/**
	 * @brief Destroys the contained object via allocator traits.
	 *
	 * @remarks Destruction is delegated to allocator_traits::destroy using the same allocator.
	 */
	~temp_value() { traits::destroy(alloc_, addr()); }

	/**
	 * @brief Returns the address of the contained object.
	 *
	 * @return Pointer to the contained T.
	 *
	 * @remarks Valid only after successful construction.
	 */
	T* addr() noexcept { return std::addressof(value_); }

	/**
	 * @brief Returns a reference to the contained object.
	 *
	 * @return Reference to the contained T.
	 *
	 * @remarks Valid only after successful construction.
	 */
	T& get() noexcept { return *addr(); }

	/**
	 * @brief Returns a const reference to the contained object.
	 *
	 * @return Const reference to the contained T.
	 *
	 * @remarks Valid only after successful construction.
	 */
	const T& get() const noexcept { return *addr(); }

private:
	/**
	 * @brief Raw storage for T, inactive until constructed.
	 *
	 * @details
	 * The union provides storage without invoking the constructor of T.
	 * allocator_traits::construct activates the object in this storage.
	 */
	union
	{
		T value_;
	};

	/**
	 * @brief Allocator used for construction and destruction.
	 */
	Alloc& alloc_;
};

SNAP_END_NAMESPACE
