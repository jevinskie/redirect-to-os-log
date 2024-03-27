#include "redirect-to-os-log/redirect-to-os-log.hpp"

exported_class::exported_class() : m_name{"redirect-to-os-log"} {}

auto exported_class::name() const -> char const * {
    return m_name.c_str();
}
