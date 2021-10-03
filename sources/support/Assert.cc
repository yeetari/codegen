#include <coel/support/Assert.hh>

#include <fmt/core.h>

#include <cstdlib>

namespace coel {

[[noreturn]] void assertion_failed(const char *file, unsigned int line, const char *expr, const char *msg) {
    fmt::print("Assertion '{}' failed at {}:{}\n", expr, file, line);
    if (msg != nullptr) {
        fmt::print("=> {}\n", msg);
    }
    std::abort();
}

} // namespace coel
