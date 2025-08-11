#pragma once

#ifndef SNAP_HAS_BUILTIN
    #if defined(__has_builtin)
        #define SNAP_HAS_BUILTIN(builtin) __has_builtin(builtin)
    #else
        #define SNAP_HAS_BUILTIN(builtin) (0)
    #endif // defined(__has_builtin)
#endif // SNAP_HAS_BUILTIN
