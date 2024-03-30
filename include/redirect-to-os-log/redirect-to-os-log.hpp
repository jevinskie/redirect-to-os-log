#pragma once

#include <unistd.h>

namespace redirect_to_os_log {

struct log_args {
    const char *const __nonnull subsystem;
    const bool is_injected;
    const bool echo;
};

[[gnu::visibility("default")]]
extern int exit_pipe[2];

[[gnu::visibility("default")]]
extern ssize_t safe_write(int fd, const void *__nonnull buf, ssize_t count);

[[gnu::visibility("default")]]
extern void *__nullable io_loop(void *__nonnull arg);

} // namespace redirect_to_os_log
