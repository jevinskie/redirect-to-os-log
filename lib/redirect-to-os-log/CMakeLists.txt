# headers added for Xcode project generation
add_library(redirect-to-os-log-lib STATIC redirect-to-os-log-lib.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../include/redirect-to-os-log/redirect-to-os-log.hpp)
set_target_properties(
    redirect-to-os-log-lib PROPERTIES
    PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../../include/redirect-to-os-log/redirect-to-os-log.hpp
    CXX_STANDARD 23
    CXX_EXTENSIONS ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME redirect-to-os-log
    OUTPUT_NAME redirect-to-os-log
)
target_compile_options(redirect-to-os-log-lib PRIVATE -Wall -Wextra -Wpedantic -Wno-nullability-extension -Wno-gnu-line-marker)
target_include_directories(redirect-to-os-log-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
