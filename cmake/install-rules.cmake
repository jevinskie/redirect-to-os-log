if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/redirect-to-os-log-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package redirect-to-os-log)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT redirect-to-os-log_Development
)

install(
    TARGETS redirect-to-os-log_redirect-to-os-log
    EXPORT redirect-to-os-logTargets
    RUNTIME #
    COMPONENT redirect-to-os-log_Runtime
    LIBRARY #
    COMPONENT redirect-to-os-log_Runtime
    NAMELINK_COMPONENT redirect-to-os-log_Development
    ARCHIVE #
    COMPONENT redirect-to-os-log_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    redirect-to-os-log_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE redirect-to-os-log_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(redirect-to-os-log_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${redirect-to-os-log_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT redirect-to-os-log_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${redirect-to-os-log_INSTALL_CMAKEDIR}"
    COMPONENT redirect-to-os-log_Development
)

install(
    EXPORT redirect-to-os-logTargets
    NAMESPACE redirect-to-os-log::
    DESTINATION "${redirect-to-os-log_INSTALL_CMAKEDIR}"
    COMPONENT redirect-to-os-log_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
