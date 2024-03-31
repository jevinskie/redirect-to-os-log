#include "redirect-to-os-log/redirect-to-os-log.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <os/log.h>
#include <pthread.h>
#include <spawn.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#undef NDEBUG
#include <cassert>

extern "C" const char *const *const environ;

extern "C" [[gnu::noreturn, gnu::cold, gnu::format(__printf__, 1, 2)]]
void abort_report_np(const char *const __nonnull fmt, ...);

struct crashreporter_annotations_t {
    uint64_t version;
    char *message;
    char *signature_string;
    char *backtrace;
    char *message2;
    uint64_t thread;
    uint32_t dialog_mode;
    uint32_t padding0;
    uint32_t abort_cause;
    uint32_t padding1;
};

extern "C" {
[[gnu::visibility("hidden"), gnu::section("__DATA,__crash_info")]]
static crashreporter_annotations_t gCRAnnotations = {.version = 5};
}

extern "C" [[gnu::noreturn, gnu::cold, gnu::format(__printf__, 2, 3)]]
void _simple_dprintf(int fd, const char *const __nonnull fmt, ...);

namespace redirect_to_os_log {

static auto CRGetCrashLogMessage() {
    return gCRAnnotations.message;
}

static void CRSetCrashLogMessage(char *msg) {
    gCRAnnotations.message = msg;
}

static int stdout_pipe[2];
static int stderr_pipe[2];
int exit_pipe[2];
static int original_stdout;
static int original_stderr;
static os_log_t logger_stdout;
static os_log_t logger_stderr;

bool should_echo_from_env_var() {
    const auto do_echo = getenv("REDIRECT_TO_OS_LOG");
    if (do_echo) {
        if (!strcmp(do_echo, "1") || !strcmp(do_echo, "YES") || !strcmp(do_echo, "ON")) {
            return true;
        }
    }
    return false;
}

void posix_check(const int retval, const char *const __nonnull msg,
                 const std::source_location location) {
    const auto cerrno = errno;
    if (REDIRECT_UNLIKELY(retval < 0)) {
        abort_report_np("POSIX error: function: %s, file %s, line %u:%u: retval: %d errno: %d aka "
                        "'%s' description: '%s'.\n",
                        location.function_name(), location.file_name(), location.line(),
                        location.column(), retval, cerrno, strerror(cerrno), msg);
    }
}

ssize_t safe_write(int fd, const void *__nonnull buf, ssize_t count) {
    assert(count >= 0);
    const auto written = count;
    auto buffer        = static_cast<const char *>(buf);

    while (count > 0) {
        ssize_t res       = write(fd, buffer, static_cast<size_t>(count));
        const auto cerrno = errno;
        if (res < 0) {
            if (cerrno == EINTR) {
                continue; // Interrupted by a signal, try again
            } else {
                posix_check(static_cast<int>(res), "safe_write");
            }
        }
        buffer += res;
        count -= res;
    }
    return written;
}

static void write_and_log(int fd, const std::string &line, bool echo) {
    const size_t max_log_length = 1024;

    if (echo) {
        safe_write(fd == STDOUT_FILENO ? STDOUT_FILENO : STDERR_FILENO, line.c_str(),
                   static_cast<ssize_t>(line.length()));
        safe_write(fd == STDOUT_FILENO ? STDOUT_FILENO : STDERR_FILENO, "\n", 1);
    }

    for (size_t pos = 0; pos < line.length(); pos += max_log_length) {
        std::string chunk = line.substr(pos, max_log_length);
        os_log_with_type(fd == STDOUT_FILENO ? logger_stdout : logger_stderr,
                         fd == STDOUT_FILENO ? OS_LOG_TYPE_DEFAULT : OS_LOG_TYPE_ERROR,
                         "%{public}s", chunk.c_str());
    }
}

static void log_output(int fd, bool echo) {
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        if (echo) {
            safe_write(fd, buffer, bytes_read);
        }
        os_log_with_type(fd == STDOUT_FILENO ? logger_stdout : logger_stderr,
                         fd == STDOUT_FILENO ? OS_LOG_TYPE_DEFAULT : OS_LOG_TYPE_ERROR,
                         "%{public}.*s", static_cast<int>(bytes_read), buffer);
    }
}

static void setup_io_redirection(const char *const __nonnull subsystem, const bool is_injected) {
    // Create pipes for stdout and stderr
    assert(!pipe(stdout_pipe));
    assert(!pipe(stderr_pipe));
    if (is_injected) {
        assert(!pipe(exit_pipe));
    }

    // Duplicate the original stdout and stderr
    original_stdout = dup(STDOUT_FILENO);
    assert(original_stdout >= 0);
    original_stderr = dup(STDERR_FILENO);
    assert(original_stderr >= 0);

    // Redirect stdout and stderr to the write ends of the pipes
    assert(dup2(stdout_pipe[1], STDOUT_FILENO) == STDOUT_FILENO);
    assert(dup2(stderr_pipe[1], STDERR_FILENO) == STDERR_FILENO);

    // Close the original write ends of the pipes
    assert(!close(stdout_pipe[1]));
    assert(!close(stderr_pipe[1]));

    logger_stdout = os_log_create(subsystem, "stdout");
    logger_stderr = os_log_create(subsystem, "stderr");
}

void *__nullable io_loop(void *__nonnull arg) {
    const auto args        = reinterpret_cast<const log_args *>(arg);
    const auto is_injected = args->is_injected;
    const auto echo        = args->echo || should_echo_from_env_var();
    setup_io_redirection(args->subsystem, is_injected);

    struct kevent kev[3];
    int kq = kqueue();
    assert(kq >= 0);
    bool do_exit = false;

    // Set up the kevents to monitor the read ends of the pipes
    EV_SET(&kev[0], stdout_pipe[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
    EV_SET(&kev[1], stderr_pipe[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (is_injected) {
        EV_SET(&kev[2], exit_pipe[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
    }
    assert(kevent(kq, kev, 2 + is_injected, nullptr, 0, nullptr) == 2 + is_injected);

    while (!do_exit) {
        // Wait for events
        struct kevent event_list[3];
        int nev = kevent(kq, nullptr, 0, event_list, 2 + is_injected, nullptr);
        assert(nev >= 0);
        for (int i = 0; i < nev; ++i) {
            int fd = static_cast<int>(event_list[i].ident);
            if (fd == stdout_pipe[0] || fd == stderr_pipe[0]) {
                log_output(fd, echo);
            }
            if (is_injected && fd == exit_pipe[0]) {
                do_exit = true;
            }
        }
    }

    assert(!close(kq));
    if (is_injected) {
        pthread_exit(nullptr);
    }
    return nullptr;
}

} // namespace redirect_to_os_log
