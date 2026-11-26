#ifndef BB_STRING_H
#define BB_STRING_H
#include <stddef.h>

#include "engine/common/bbFlag.h"
/// Copy a string into a bounded buffer.
/// Returns bbFull if the string is truncated.
/// Guarantees null termination when capacity > 0.
static inline bbFlag bbStr_setStr(char *dest, const char *src, size_t capacity) {
    if (capacity == 0) return bbFull;

    for (size_t i = 0; i < capacity; i++) {
        dest[i] = src[i];
        if (src[i] == '\0') return bbSuccess;
    }

    dest[capacity - 1] = '\0';
    return bbFull;
}

#endif //BB_STRING_H
