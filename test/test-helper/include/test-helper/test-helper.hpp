#pragma once

#include <string_view>

[[gnu::visibility("default")]]
bool write_to_fd(int fd, std::string_view str);
