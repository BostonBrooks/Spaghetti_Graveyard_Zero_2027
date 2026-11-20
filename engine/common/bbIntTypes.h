
#ifndef BB_INTTYPES_H
#define BB_INTTYPES_H

#include <stdint.h>
#include <limits.h>

typedef int8_t   I8;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

#define I32_MAX INT32_MAX
#define I32_MIN INT32_MIN
#define U64_MAX UINT64_MAX

#endif // BB_INTTYPES_H
