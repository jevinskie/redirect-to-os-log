add_executable(redirect-to-os-log-tool-test-subprocess redirect-to-os-log-tool-test-subprocess.cpp)
target_compile_features(redirect-to-os-log-tool-test-subprocess PRIVATE cxx_std_20)
set_target_properties(redirect-to-os-log-tool-test-subprocess PROPERTIES CXX_EXTENSIONS ON)
target_compile_options(redirect-to-os-log-tool-test-subprocess PRIVATE -Wall -Wextra -Wpedantic -Wno-gnu-line-marker)
target_link_libraries(redirect-to-os-log-tool-test-subprocess PRIVATE test-helper)
add_dependencies(redirect-to-os-log-tool-test-subprocess redirect-to-os-log)

add_test(NAME redirect-to-os-log-tool-test-subprocess COMMAND redirect-to-os-log-tool-test-subprocess)


add_executable(redirect-to-os-log-tool-test redirect-to-os-log-tool-test.cpp)
target_compile_features(redirect-to-os-log-tool-test PRIVATE cxx_std_20)
set_target_properties(redirect-to-os-log-tool-test PROPERTIES CXX_EXTENSIONS ON)
target_compile_options(redirect-to-os-log-tool-test PRIVATE -Wall -Wextra -Wpedantic -Wno-gnu-line-marker)
add_dependencies(redirect-to-os-log-tool-test redirect-to-os-log redirect-to-os-log-tool-test-subprocess)

add_test(NAME redirect-to-os-log-tool-test COMMAND redirect-to-os-log-tool-test)
