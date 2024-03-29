add_executable(redirect-to-os-log-lib-test redirect-to-os-log-lib-test.cpp)
target_compile_features(redirect-to-os-log-lib-test PRIVATE cxx_std_23)
set_target_properties(redirect-to-os-log-lib-test PROPERTIES CXX_EXTENSIONS ON)
target_compile_options(redirect-to-os-log-lib-test PRIVATE -Wall -Wextra -Wpedantic -Wno-nullability-extension -Wno-gnu-line-marker)
target_link_libraries(redirect-to-os-log-lib-test PRIVATE redirect-to-os-log-lib)

add_test(NAME redirect-to-os-log-lib-test COMMAND redirect-to-os-log-lib-test)
