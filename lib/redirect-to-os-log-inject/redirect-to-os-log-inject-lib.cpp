#include <redirect-to-os-log/redirect-to-os-log.hpp>

#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#undef NDEBUG
#include <cassert>

namespace redirect_to_os_log_inject {

static pthread_t io_loop_thread;

[[gnu::constructor]]
static void redirect_to_os_log_injector_ctor() {
    redirect_to_os_log::setup_io_redirection(true);

    // Spawn the I/O loop thread
    pthread_create(&io_loop_thread, nullptr, redirect_to_os_log::io_loop, nullptr);
}

[[gnu::destructor]]
static void redirect_to_os_log_injector_dtor() {
    // Signal the I/O loop thread to exit by writing to the pipe
    redirect_to_os_log::safe_write(redirect_to_os_log::exit_pipe[1], "x",
                                   1); // The actual value written is irrelevant

    // Wait for the thread to exit
    pthread_join(io_loop_thread, nullptr);

    // Close the pipe
    assert(!close(redirect_to_os_log::exit_pipe[0]));
    assert(!close(redirect_to_os_log::exit_pipe[1]));
}

} // namespace redirect_to_os_log_inject
