#ifndef MEMORY_H
#define MEMORY_H

void memcpy(void *dest, void *src, unsigned int size);

static inline void *memset(void *s, int c, unsigned int len)
{
    unsigned char *p = s;
    while (len--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}

#endif