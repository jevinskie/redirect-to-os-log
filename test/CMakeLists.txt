cmake_minimum_required(VERSION 3.28)

project(redirect-to-os-log-tests LANGUAGES CXX)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(redirect-to-os-log REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_subdirectory(test-helper)
add_subdirectory(lib)
add_subdirectory(tool)

# ---- End-of-file commands ----

add_folders(test)
