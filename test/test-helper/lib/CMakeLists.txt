# headers added for Xcode project generation
add_library(test-helper STATIC test-helper.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../include/test-helper/test-helper.hpp)
target_compile_features(test-helper PUBLIC cxx_std_20)
set_target_properties(
    test-helper PROPERTIES
    PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../include/test-helper/test-helper.hpp
    CXX_EXTENSIONS ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)
target_compile_options(test-helper PRIVATE -Wall -Wextra -Wpedantic -Wno-nullability-extension -Wno-gnu-line-marker)
target_include_directories(test-helper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
