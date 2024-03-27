add_executable(redirect-to-os-log-inject-lib-test redirect-to-os-log-inject-lib-test.cpp)
target_compile_features(redirect-to-os-log-inject-lib-test PRIVATE cxx_std_20)
set_target_properties(redirect-to-os-log-inject-lib-test PROPERTIES CXX_EXTENSIONS ON)
target_compile_options(redirect-to-os-log-inject-lib-test PRIVATE -Wall -Wextra -Wpedantic -Wno-gnu-line-marker)
target_link_libraries(redirect-to-os-log-inject-lib-test PRIVATE test-helper)

add_test(NAME redirect-to-os-log-inject-lib-test COMMAND redirect-to-os-log-inject-lib-test)
