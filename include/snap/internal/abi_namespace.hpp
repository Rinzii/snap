#ifndef SNP_INCLUDE_SNAP_INTERNAL_ABI_NAMESPACE_HPP
#define SNP_INCLUDE_SNAP_INTERNAL_ABI_NAMESPACE_HPP

#if defined(__has_include) && __has_include("snap/internal/config/abi_namespace.hpp")
	#include "snap/internal/config/abi_namespace.hpp"
#else
	#ifndef SNAP_ENABLE_ABI_NAMESPACE
		#define SNAP_ENABLE_ABI_NAMESPACE 1
	#endif
	#ifndef SNAP_ABI_NAMESPACE
		#define SNAP_ABI_NAMESPACE abi_v1
	#endif
	#ifndef SNAP_NAMESPACE
		#define SNAP_NAMESPACE snap
	#endif
#endif

#if SNAP_ENABLE_ABI_NAMESPACE
	#define SNAP_BEGIN_NAMESPACE                                                                                                                               \
		namespace SNAP_NAMESPACE                                                                                                                               \
		{                                                                                                                                                      \
			inline namespace SNAP_ABI_NAMESPACE                                                                                                                \
			{
	#define SNAP_END_NAMESPACE                                                                                                                                 \
		}                                                                                                                                                      \
		} // namespace SNAP_NAMESPACE
#else
	#define SNAP_BEGIN_NAMESPACE                                                                                                                               \
		namespace SNAP_NAMESPACE                                                                                                                               \
		{
	#define SNAP_END_NAMESPACE } // namespace SNAP_NAMESPACE
#endif

#endif // SNP_INCLUDE_SNAP_INTERNAL_ABI_NAMESPACE_HPP
