/// Handle is used to address elements of pool, hold value in key-value pairs in dictionary, etc

#ifndef BB_HANDLE_H
#define BB_HANDLE_H
#include "bbIntTypes.h"


typedef struct
{
    U32 index;
    U16 system;
    U16 generation;
} bbSystem_Handle;

typedef union {
    void* ptr;
    U64 u64;
    bbSystem_Handle system;
    I64 i64;
} bbHandle;


#endif //BB_HANDLE_H