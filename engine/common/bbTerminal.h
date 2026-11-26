/// Macros here are used to print text to the terminal

#ifndef BB_TERMINAL_H
#define BB_TERMINAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <threads.h>

#include "engine/timekeeping/bbTime.h"

extern _Thread_local bool bbWarningsAreFatal;
extern _Thread_local  bool debug_off;
extern _Thread_local  char* thread;
extern bbTime test_time;

#ifdef _WIN32

#define bbLockStderr()   _lock_file(stderr)
#define bbUnlockStderr() _unlock_file(stderr)

#else

#define bbLockStderr()   flockfile(stderr)
#define bbUnlockStderr() funlockfile(stderr)

#endif

#define bbAssert(expression, ...)\
do {\
if (!(expression)){\
bbLockStderr();\
fprintf (stderr, "In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s"\
"\nASSERTION: %s, ", __FILE_NAME__, __func__, __LINE__, thread,\
#expression );\
fprintf(stderr, __VA_ARGS__);\
fflush(stderr);\
bbUnlockStderr();\
\
abort();\
}\
} while(0)

#define bbWarning(expression, ...)\
do {\
if (!(expression)){\
bbLockStderr();\
fprintf (stderr, "In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s"\
"\nWARNING: %s, ", __FILE_NAME__, __func__, __LINE__, thread,\
#expression );\
fprintf(stderr, __VA_ARGS__);\
fflush(stderr);\
bbUnlockStderr();\
if (bbWarningsAreFatal) {\
abort();\
}\
}\
} while(0)

#define bbNotHere()\
do {\
fprintf (stderr, "In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s\n"\
"BB_NOT_HERE\n", __FILE_NAME__, __func__, __LINE__, thread);\
fflush(stderr);\
abort();\
} while(0)

#define bbNotImplemented()\
    printf ("In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s, TIME: %lu\nBB_NOT_IMPLEMENTED\n",\
__FILE_NAME__, __func__, __LINE__, thread, test_time);


#define bbHere()\
do {\
if(!debug_off){\
fprintf (stdout, "In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s\n"\
"BB_HERE\n", __FILE_NAME__, __func__, __LINE__, thread);\
fflush(stdout);\
}\
} while(0)

#define bbDebug(...)\
do {\
if(!debug_off){\
fprintf (stdout, "In FILE: %s, FUNCTION: %s, LINE: %d, THREAD: %s\n"\
"DEBUG: ", __FILE_NAME__, __func__, __LINE__, thread);\
fprintf(stdout, __VA_ARGS__);\
fflush(stdout);\
}\
} while(0)
#endif //BB_TERMINAL_H