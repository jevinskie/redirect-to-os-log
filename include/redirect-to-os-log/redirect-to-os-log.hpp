#pragma once

#include <string>

/**
 * @brief Reports the name of the library
 *
 * Please see the note above for considerations when creating shared libraries.
 */
class [[gnu::visibility("default")]] exported_class {
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
