#include <redirect-to-os-log/redirect-to-os-log.hpp>

int main(int argc, const char *const *argv) {

    // Spawn the I/O loop thread
    bool is_injected = false;
    redirect_to_os_log::io_loop(reinterpret_cast<void *>(is_injected));

    return 0;
}
