// libc/include/memset.h
#ifndef LIBC_MEMSET_H
#define LIBC_MEMSET_H

#include <stddef.h> // For size_t

void* memset(void* bufptr, int value, size_t size);

#endif // LIBC_MEMSET_H
