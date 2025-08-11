if (NOT DEFINED SNAP_ROOT_DIR)
    message(FATAL_ERROR "SNAP_ROOT_DIR not defined in cmake for install rules!")
endif ()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY
        "${SNAP_ROOT_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.hpp"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/snap/version.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/snap"
)

install(
        TARGETS ${SNAP_TARGET_NAME}
        EXPORT ${SNAP_TARGET_NAME}-targets
)

install(EXPORT ${SNAP_TARGET_NAME}-targets
        FILE ${SNAP_TARGET_NAME}-targets.cmake
        NAMESPACE ${SNAP_TARGET_NAME}::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/snap"
)

configure_package_config_file(
        "${SNAP_ROOT_DIR}/cmake/in/snap-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/snap-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/snap"
)

# Write a version file for strict version checking
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/snap-config-version.cmake"
        COMPATIBILITY SameMajorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/snap-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/snap-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/snap"
)
