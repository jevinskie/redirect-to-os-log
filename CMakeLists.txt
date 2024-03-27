cmake_minimum_required(VERSION 3.28)

include(cmake/prelude.cmake)

project(
    redirect-to-os-log
    VERSION 0.1.0
    DESCRIPTION "Command line utility to run a program and redirect its stdout and stderr to XNU's os_log facilities. Also includes an injectable dylib for the same purpose."
    HOMEPAGE_URL "https://github.com/jevinskie/redirect-to-os-log"
    LANGUAGES CXX
)

include(cmake/variables.cmake)

add_subdirectory(lib)
add_subdirectory(tool)
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(test)
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  if(PROJECT_IS_TOP_LEVEL)
    include(CPack)
  endif()
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
