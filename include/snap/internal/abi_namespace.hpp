#pragma once

#include "snap/internal/pp/has_include.hpp"

#if SNAP_HAS_INCLUDE("snap/internal/config/abi_namespace.hpp")
	#include "snap/internal/config/abi_namespace.hpp"
#else
	#ifndef SNAP_ENABLE_ABI_NAMESPACE
		#define SNAP_ENABLE_ABI_NAMESPACE 1
	#endif
	#ifndef SNAP_ABI_NAMESPACE
		#define SNAP_ABI_NAMESPACE abi_v1
	#endif
#endif

#if SNAP_ENABLE_ABI_NAMESPACE
	#define SNAP_BEGIN_NAMESPACE                                                                                                                               \
		namespace snap                                                                                                                                         \
		{                                                                                                                                                      \
			inline namespace SNAP_ABI_NAMESPACE                                                                                                                \
			{
	#define SNAP_END_NAMESPACE                                                                                                                                 \
		}                                                                                                                                                      \
		} // namespace snap
#else
	#define SNAP_BEGIN_NAMESPACE                                                                                                                               \
		namespace snap                                                                                                                                         \
		{
	#define SNAP_END_NAMESPACE } // namespace snap
#endif
