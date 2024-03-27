#include "test-helper/test-helper.hpp"

#undef NDEBUG
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>

bool write_to_fd(int fd, std::string_view str) {
    const auto sz     = str.size();
    const auto buf    = str.data();
    size_t sz_written = 0;
    bool done_in_one  = false;
    while (sz_written < sz) {
        const auto res = write(fd, buf + sz_written, sz - sz_written);
        if (res < 0) {
            const auto cerrno = errno;
            if (cerrno == EINTR) {
                continue;
            }
            fprintf(stderr, "write of '%*s' to fd %d failed with errno %d aka '%s'\n",
                    static_cast<int>(sz - sz_written), buf + sz_written, fd, cerrno,
                    strerror(cerrno));
            assert(0 && "write_to_fd failed");
        } else {
            const auto res_as_sz = static_cast<decltype(sz_written)>(res);
            if (sz_written == 0 && res_as_sz == sz) {
                done_in_one = true;
            }
            sz_written += res_as_sz;
        }
    }
    return done_in_one;
}
