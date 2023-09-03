#ifndef ASSERT_H
#define ASSERT_H

#include "print.h"

#define PANDOS_ASSERT(cond, msg, ...)                            \
    if (!(cond))                                                   \
    {                                                            \
        PRINT_ERROR(msg, ##__VA_ARGS__);                         \
        printf("\nAt line %d in file " __FILE__ "\n", __LINE__); \
        PANIC();                                                 \
    }

#endif