#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <iostream>
#include <cstdlib>

#define ASSERT(condition)                                                     \
    do {                                                                      \
        if (!(condition)) {                                                   \
            std::cerr << "Assertion failed: (" #condition ") in "             \
                      << __FILE__ << ", line " << __LINE__                    \
                      << ", function " << __func__ << std::endl;              \
            std::abort();                                                     \
        }                                                                     \
    } while (0)

#define PANIC(msg)                                                           \
    do {                                                                     \
        std::cerr << "Panic: " << msg << " in " << __FILE__ << ", line "     \
                  << __LINE__ << ", function " << __func__ << std::endl;      \
        std::abort();                                                        \
    } while (0)

#endif // __ASSERT_H__
