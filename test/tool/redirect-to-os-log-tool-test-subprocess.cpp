#undef NDEBUG
#include <cassert>
#include <unistd.h>

#include <test-helper/test-helper.hpp>

auto main() -> int {
    assert(write_to_fd(STDOUT_FILENO, "stdout full line\n"));
    assert(write_to_fd(STDOUT_FILENO, "stdout partial line part 1 - "));
    assert(write_to_fd(STDOUT_FILENO, "stdout partial line part 2\n"));
    assert(write_to_fd(STDERR_FILENO, "stderr full line\n"));
    assert(write_to_fd(STDERR_FILENO, "stderr partial line part 1 - "));
    assert(write_to_fd(STDERR_FILENO, "stderr partial line part 2\n"));
    assert(write_to_fd(STDOUT_FILENO, "stdout intermixed partial line part 1 - "));
    assert(write_to_fd(STDERR_FILENO, "stderr intermixed partial line part 1 - "));
    assert(write_to_fd(STDOUT_FILENO, "stdout intermixed partial line part 2\n"));
    assert(write_to_fd(STDERR_FILENO, "stderr intermixed partial line part 2\n"));
    assert(write_to_fd(STDOUT_FILENO, "stdout final partial line"));
    assert(write_to_fd(STDERR_FILENO, "stderr final partial line"));
    return 0;
}
