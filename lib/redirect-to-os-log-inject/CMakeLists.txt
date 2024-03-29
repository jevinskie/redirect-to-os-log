add_library(redirect-to-os-log-inject SHARED redirect-to-os-log-inject-lib.cpp)
target_compile_features(redirect-to-os-log-inject PRIVATE cxx_std_20)
set_target_properties(redirect-to-os-log-inject PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS ON
)
target_compile_options(redirect-to-os-log-inject PRIVATE -Wall -Wextra -Wpedantic -Wno-nullability-extension -Wno-gnu-line-marker)
target_link_libraries(redirect-to-os-log-inject PRIVATE redirect-to-os-log-lib)
