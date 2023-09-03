#ifndef PRINT_H
#define PRINT_H

#define DEBUG

extern int breakpoint();

#ifdef DEBUG

    #define FORMAT_DEBUG(x, y) x y

    #define PRINT_DEBUG(x, ...) printf(FORMAT_DEBUG("["__FILE__",%d]>>> ",x),__LINE__, ##__VA_ARGS__)
    #define PRINT_PANDOS(x, ...) printf(FORMAT_DEBUG("[PANDOS]", x), ##__VA_ARGS__)
    #define PRINT_ERROR(x, ...) printf(FORMAT_DEBUG("[ERROR]", x), ##__VA_ARGS__)

    #define BREAKPOINT()                                                                                                       \
    {                                                                                                                      \
        printf("Breakpoint in file " __FILE__ " at line %d, func: %s\n", __LINE__, __func__);                              \
        breakpoint();                                                                                                      \
    }

    #define panic(x, ...)                                                                                                  \
    {                                                                                                                  \
        printf(FORMAT_DEBUG("\n[ERROR] Panic at line %d, func: %s\nMessage: ", x), __LINE__, __func__, ##__VA_ARGS__); \
        PANIC();                                                                                                       \
    }

#else

    #define PRINT_DEBUG(x, ...) ;
    #define PRINT_PANDOS(x, ...) ;
    #define PRINT_ERROR(x, ...) ;
    #define BREAKPOINT() ;
    #define panic(x, ...) PANIC();

#endif

#define va_arg(varg, type) (type) * ((type *)varg++)
#define va_list int *
#define va_start(varg, fmt) varg = (int *)(&fmt + 1)
#define va_end(varg)

void addokbuf(char *strp);
int printf(char *str, ...);

#endif