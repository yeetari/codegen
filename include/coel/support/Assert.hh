#pragma once

#ifndef NDEBUG
#define COEL_ASSERTIONS
#define COEL_ASSERTIONS_PEDANTIC
#endif

#define COEL_ENSURE(expr, ...)                                                                                         \
    static_cast<bool>(expr) ? static_cast<void>(0) : coel::assertion_failed(__FILE__, __LINE__, #expr, ##__VA_ARGS__)
#ifdef COEL_ASSERTIONS
#define COEL_ASSERT(expr, ...) COEL_ENSURE(expr, ##__VA_ARGS__)
#else
#define COEL_ASSERT(expr, ...)
#endif
#ifdef COEL_ASSERTIONS_PEDANTIC
#define COEL_ASSERT_PEDANTIC(expr, ...) COEL_ENSURE(expr, ##__VA_ARGS__)
#else
#define COEL_ASSERT_PEDANTIC(expr, ...)
#endif

#define COEL_ASSERT_NOT_REACHED(...) COEL_ASSERT(false, ##__VA_ARGS__)
#define COEL_ENSURE_NOT_REACHED(...) COEL_ENSURE(false, ##__VA_ARGS__)

namespace coel {

[[noreturn]] void assertion_failed(const char *file, unsigned int line, const char *expr, const char *msg = nullptr);

} // namespace coel
