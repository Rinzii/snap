#pragma once

#ifndef SNAP_TRIVIAL_ABI_ATTR
	#if defined(__clang__)
		#define SNAP_TRIVIAL_ABI_ATTR [[clang::trivial_abi]]
	#else
		#define SNAP_TRIVIAL_ABI_ATTR
	#endif
#endif