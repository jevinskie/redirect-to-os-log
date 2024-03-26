cmake_minimum_required(VERSION 3.14)

project(redirect-to-os-logTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(redirect-to-os-log REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(redirect-to-os-log_test source/redirect-to-os-log_test.cpp)
target_link_libraries(redirect-to-os-log_test PRIVATE redirect-to-os-log::redirect-to-os-log)
target_compile_features(redirect-to-os-log_test PRIVATE cxx_std_20)

add_test(NAME redirect-to-os-log_test COMMAND redirect-to-os-log_test)

# ---- End-of-file commands ----

add_folders(Test)
