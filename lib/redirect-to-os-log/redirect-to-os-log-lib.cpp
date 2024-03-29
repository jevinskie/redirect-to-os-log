#include "redirect-to-os-log/redirect-to-os-log.hpp"

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

namespace redirect_to_os_log {

static int stdout_pipe[2];
static int stderr_pipe[2];
int exit_pipe[2];
static int original_stdout;
static int original_stderr;

ssize_t safe_write(int fd, const void *__nonnull buf, ssize_t count) {
    const auto written = count;
    auto buffer        = static_cast<const char *>(buf);
    assert(count >= 0);

    while (count > 0) {
        ssize_t res = write(fd, buffer, static_cast<size_t>(count));
        if (res < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by a signal, try again
            } else {
                assert(false && "safe_write failed");
            }
        }
        buffer += res;
        count -= res;
    }
    return written;
}

void log_output(int fd, os_log_t logger, bool echo) {
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        if (echo) {
            safe_write(fd == STDOUT_FILENO ? STDOUT_FILENO : STDERR_FILENO, buffer, bytes_read);
        }
        os_log_with_type(logger, fd == STDOUT_FILENO ? OS_LOG_TYPE_DEFAULT : OS_LOG_TYPE_ERROR,
                         "%{public}s", buffer);
    }
}

void setup_io_redirection(const bool is_injected) {
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
}

void *__nullable io_loop(void *__nonnull arg) {
    const auto is_injected = *reinterpret_cast<bool *>(arg);
    struct kevent kev[3];
    int kq = kqueue();
    assert(kq >= 0);

    // Set up the kevents to monitor the read ends of the pipes
    EV_SET(&kev[0], stdout_pipe[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&kev[1], stderr_pipe[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (is_injected) {
        EV_SET(&kev[2], exit_pipe[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
    }
    kevent(kq, kev, 2 + is_injected, NULL, 0, NULL);

    while (true) {
        // Wait for events
        struct kevent event_list[2];
        int nev = kevent(kq, NULL, 0, event_list, 2, NULL);
        for (int i = 0; i < nev; i++) {
            int fd = event_list[i].ident;
            if (fd == stdout_pipe[0] || fd == stderr_pipe[0]) {
                // Data is available to read, echo it to the original stdout or stderr
                char buffer[1024];
                ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    int output_fd = (fd == stdout_pipe[0]) ? original_stdout : original_stderr;
                    safe_write(output_fd, buffer, bytes_read);
                }
            }
        }
    }

    close(kq);
    pthread_exit(nullptr);
    return nullptr;
}

} // namespace redirect_to_os_log
