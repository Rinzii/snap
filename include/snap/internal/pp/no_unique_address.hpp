#pragma once

#if defined(__has_cpp_attribute)
	#if defined(_MSC_VER) && __has_cpp_attribute(msvc::no_unique_address)
		#define SNAP_NO_UNIQUE_ADDRESS_ATTR [[msvc::no_unique_address]]
	#elif __has_cpp_attribute(no_unique_address) && __cplusplus >= 202002L
		#define SNAP_NO_UNIQUE_ADDRESS_ATTR [[no_unique_address]]
	#else
		#define SNAP_NO_UNIQUE_ADDRESS_ATTR
	#endif
#else
	#if defined(_MSC_VER) && __cplusplus >= 202002L
		#define SNAP_NO_UNIQUE_ADDRESS_ATTR [[msvc::no_unique_address]]
	#else
		#define SNAP_NO_UNIQUE_ADDRESS_ATTR
	#endif
#endif
