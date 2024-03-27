#include <string>

#include "redirect-to-os-log/redirect-to-os-log.hpp"

auto main() -> int {
    auto const exported = exported_class{};

    return std::string("redirect-to-os-log") == exported.name() ? 0 : 1;
}
