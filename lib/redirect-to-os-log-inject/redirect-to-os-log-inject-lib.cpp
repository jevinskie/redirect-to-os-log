#include <cstdio>

[[gnu::constructor]]
void redirect_to_os_log_injector_ctor() {
    fprintf(stderr, "redirect_to_os_log_injector_ctor called\n");
}
