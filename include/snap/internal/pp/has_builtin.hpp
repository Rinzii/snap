#pragma once

#ifndef SNAP_HAS_BUILTIN
	#if defined(__has_builtin)
		#define SNAP_HAS_BUILTIN(BUILTIN) __has_builtin(BUILTIN)
	#else
		#define SNAP_HAS_BUILTIN(BUILTIN) (0)
	#endif // defined(__has_builtin)
#endif	   // SNAP_HAS_BUILTIN
