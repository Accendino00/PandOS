#ifndef PRINT_H
#define PRINT_H

#define PRINT_DEBUG(x, ...) printf(FORMAT_DEBUG(x), __LINE__, ##__VA_ARGS__)
#define FORMAT_DEBUG(x) "["__FILE__ ",%d]>>> " x
#define PRINT_PANDOS(x, ...) printf(FORMAT_PANDOS(x), ##__VA_ARGS__)
#define FORMAT_PANDOS(x) "[PANDOS] " x
#define PRINT_ERROR(x, ...) printf(FORMAT_ERROR(x), ##__VA_ARGS__)
#define FORMAT_ERROR(x) "[ERROR] " x

#define va_arg(varg, type) (type) * ((type *)varg++)
#define va_list int *
#define va_start(varg, fmt) varg = (int *)(&fmt + 1)
#define va_end(varg)

void addokbuf(char *strp);
int printf(char *str, ...);

#endif