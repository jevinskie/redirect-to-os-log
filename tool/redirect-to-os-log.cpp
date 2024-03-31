#include <redirect-to-os-log/redirect-to-os-log.hpp>

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <optional>
#include <spawn.h>
#include <sstream>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

namespace fs = std::filesystem;

static void print_usage() {
    fprintf(stderr, "Usage: %s [-e(cho)] <executable> <optional executable arguments>\n",
            getprogname());
}

// retval: 0 - in group, 1 - not in group, 2 - getgroups error
static int am_member_of_group(gid_t gid) {
    gid_t groups[NGROUPS_MAX + 2];
    groups[0]         = getegid();
    const int ngroups = getgroups(NGROUPS_MAX + 1, groups + 1);
    if (ngroups < 0) {
        const auto cerrno = errno;
        fprintf(stderr, "Error calling getgroups()! res: %d errno: %d aka '%s'\n", ngroups, cerrno,
                strerror(cerrno));
        return 2;
    }
    for (int i = 0; i < ngroups + 1; ++i) {
        if (groups[i] == gid) {
            return 0;
        }
    }
    return 1;
}

// retval: 0 - is executable, 1 - not executable, 2 - doesn't exist, 3 - not regular file, 4 - stat
// error, 5 - other error
static int is_exe_executable(const fs::path &exe_path, bool quiet = true) {
    std::error_code ec;
    fs::file_status fst = fs::status(exe_path, ec);
    if (ec) {
        fprintf(stderr, "Error getting status of '%s' error code: %d aka '%s'\n", exe_path.c_str(),
                ec.value(), strerror(ec.value()));
        return 5;
    }
    const auto ftype = fst.type();
    if (ftype == fs::file_type::not_found) {
        if (!quiet) {
            fprintf(stderr, "Executable '%s' doesn't exist.\n", exe_path.c_str());
        }
        return 2;
    }
    if (ftype != fs::file_type::regular) {
        if (!quiet) {
            fprintf(stderr, "Executable '%s' is not a regular file.\n", exe_path.c_str());
        }
        return 3;
    }
    struct stat st;
    int stat_res;
    if ((stat_res = stat(exe_path.c_str(), &st)) < 0) {
        if (!quiet) {
            const auto cerrno = errno;
            fprintf(stderr, "Failed to stat '%s' stat() res: %d errno: %d aka '%s'\n",
                    exe_path.c_str(), stat_res, cerrno, strerror(cerrno));
        }
        return 4;
    }
    const auto fperms = fst.permissions();
    if ((fperms & fs::perms::others_exec) != fs::perms::none) {
        return 0;
    }
    if (((fperms & fs::perms::owner_exec) != fs::perms::none) && geteuid() == st.st_uid) {
        return 0;
    }
    if (((fperms & fs::perms::group_exec) != fs::perms::none) && !am_member_of_group(st.st_gid)) {
        return 0;
    }
    return 1;
}

static std::optional<fs::path> is_exe_in_path(const fs::path &exe_path) {
    if (const auto path_var_cstr = getenv("PATH")) {
        const auto path_var = std::string{path_var_cstr};
        auto path_sstream   = std::stringstream{path_var};
        std::string path_part;
        while (std::getline(path_sstream, path_part, ':')) {
            const auto test_exe_path = fs::path{path_part} / exe_path;
            if (!is_exe_executable(test_exe_path, true)) {
                return test_exe_path;
            }
        }
    }
    return {};
}

int main(int argc, const char *const *argv) {
    bool do_echo = false;
    if (argc < 2) {
        print_usage();
        return 1;
    }
    if (!strcmp(argv[1], "-e")) {
        do_echo = true;
        if (argc < 3) {
            print_usage();
            return 1;
        }
    }
    fs::path exe_path{argv[1 + do_echo]};
    if (is_exe_executable(exe_path)) {
        std::error_code canon_ec;
#if __has_feature(cxx_exceptions)
        try {
#endif
            exe_path = fs::canonical(exe_path, canon_ec);
#if __has_feature(cxx_exceptions)
        } catch (const fs::filesystem_error &e) {
            fprintf(stderr, "Couldn't canonicalize exe path '%s' reason: '%s'\n", exe_path.c_str(),
                    e.what());
            return 2;
        }
#endif
        if (canon_ec) {
            fprintf(stderr, "Couldn't canonicalize exe path '%s' errno: %d aka '%s'\n",
                    exe_path.c_str(), canon_ec.value(), strerror(canon_ec.value()));
            return 2;
        }
    } else {
        if (auto exe_from_path_var = is_exe_in_path(exe_path)) {
            exe_path = *exe_from_path_var;
        } else {
            fprintf(stderr, "Couldn't find exe '%s' in PATH\n", exe_path.c_str());
            return 2;
        }
    }

    const auto exe_argv  = &argv[2 + do_echo];
    const auto subsystem = exe_path.filename().c_str();

    posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);

    // Run the I/O loop thread
    redirect_to_os_log::log_args args = {
        .is_injected = false, .echo = do_echo, .subsystem = subsystem};
    redirect_to_os_log::io_loop(reinterpret_cast<void *>(&args));

    return 0;
}
