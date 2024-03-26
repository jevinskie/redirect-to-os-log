cmake_minimum_required(VERSION 3.28)

include(cmake/prelude.cmake)

project(
    redirect-to-os-log
    VERSION 0.1.0
    DESCRIPTION "Command line utility to run a program and redirect its stdout and stderr to XNU's os_log facilities. Also includes an injectable dylib for the same purpose."
    HOMEPAGE_URL "https://github.com/jevinskie/redirect-to-os-log"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    redirect-to-os-log_redirect-to-os-log
    source/redirect-to-os-log.cpp
)
add_library(redirect-to-os-log::redirect-to-os-log ALIAS redirect-to-os-log_redirect-to-os-log)

include(GenerateExportHeader)
generate_export_header(
    redirect-to-os-log_redirect-to-os-log
    BASE_NAME redirect-to-os-log
    EXPORT_FILE_NAME export/redirect-to-os-log/redirect-to-os-log_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(redirect-to-os-log_redirect-to-os-log PUBLIC REDIRECT_TO_OS_LOG_STATIC_DEFINE)
endif()

set_target_properties(
    redirect-to-os-log_redirect-to-os-log PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME redirect-to-os-log
    OUTPUT_NAME redirect-to-os-log
)

target_include_directories(
    redirect-to-os-log_redirect-to-os-log ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    redirect-to-os-log_redirect-to-os-log SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(redirect-to-os-log_redirect-to-os-log PUBLIC cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT redirect-to-os-log_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of redirect-to-os-log"
  )
endif()

include(cmake/dev-mode.cmake)
