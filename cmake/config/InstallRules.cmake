if (NOT DEFINED SNAP_ROOT_DIR)
    message(FATAL_ERROR "SNAP_ROOT_DIR not defined in cmake for install rules!")
endif ()

include(GNUInstallDirs)

install(DIRECTORY
        "${SNAP_ROOT_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.hpp"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/snap/version.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/snap"
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/snap/internal/config/abi_namespace.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/snap/internal/config"
)

install(
        TARGETS ${SNAP_TARGET_NAME}
        EXPORT ${SNAP_TARGET_NAME}-targets
)

install(EXPORT ${SNAP_TARGET_NAME}-targets
        FILE ${SNAP_TARGET_NAME}-targets.cmake
        NAMESPACE ${SNAP_TARGET_NAME}::
        DESTINATION "${SNAP_CMAKE_CONFIG_INSTALL_DIR}"
)

install(FILES
        "${SNAP_CMAKE_CONFIG_BUILD_DIR}/snap-config.cmake"
        "${SNAP_CMAKE_CONFIG_BUILD_DIR}/snap-config-version.cmake"
        DESTINATION "${SNAP_CMAKE_CONFIG_INSTALL_DIR}"
)
