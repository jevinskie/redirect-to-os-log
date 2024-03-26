#pragma once

#include <string>

#include "redirect-to-os-log/redirect-to-os-log_export.hpp"

/**
 * @brief Reports the name of the library
 *
 * Please see the note above for considerations when creating shared libraries.
 */
class REDIRECT_TO_OS_LOG_EXPORT exported_class {
public:
    /**
     * @brief Initializes the name field to the name of the project
     */
    exported_class();

    /**
     * @brief Returns a non-owning pointer to the string stored in this class
     */
    auto name() const -> char const *;

private:
    std::string m_name;
};
