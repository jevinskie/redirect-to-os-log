#pragma once

#include <source_location>
#include <unistd.h>

#define REDIRECT_LIKELY(cond) __builtin_expect((cond), 1)
#define REDIRECT_UNLIKELY(cond) __builtin_expect((cond), 0)

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

[[gnu::visibility("default")]]
extern void posix_check(const int retval, const char *const __nonnull msg,
                        const std::source_location location = std::source_location::current());

} // namespace redirect_to_os_log
