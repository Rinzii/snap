#pragma once

#ifndef SNAP_HAS_INCLUDE
	#ifdef __has_include
		#define SNAP_HAS_INCLUDE(INCLUDE) __has_include(INCLUDE)
	#else
		#define SNAP_HAS_INCLUDE(INCLUDE) (0)
	#endif
#endif
