#pragma once

#include <unistd.h>

namespace redirect_to_os_log {

[[gnu::visibility("default")]]
extern int exit_pipe[2];

[[gnu::visibility("default")]]
extern ssize_t safe_write(int fd, const void *__nonnull buf, ssize_t count);

[[gnu::visibility("default")]]
void *__nullable io_loop(void *__nonnull arg);

} // namespace redirect_to_os_log
