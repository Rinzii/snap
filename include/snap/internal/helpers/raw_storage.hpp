#ifndef SNP_INCLUDE_SNAP_INTERNAL_HELPERS_RAW_STORAGE_HPP
#define SNP_INCLUDE_SNAP_INTERNAL_HELPERS_RAW_STORAGE_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cstddef>
#include <type_traits>

SNAP_BEGIN_NAMESPACE

namespace internal
{
	template <std::size_t Size, std::size_t Alignment> struct raw_storage
	{
		static_assert(Alignment != 0, "Alignment must be non-zero");
		static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of two");

		alignas(Alignment) unsigned char buffer[Size];

		void* data() noexcept { return static_cast<void*>(buffer); }
		const void* data() const noexcept { return static_cast<const void*>(buffer); }
	};

	template <class T> using raw_storage_for = raw_storage<sizeof(T), alignof(T)>;
} // namespace internal

SNAP_END_NAMESPACE

#endif // SNP_INCLUDE_SNAP_INTERNAL_HELPERS_RAW_STORAGE_HPP
