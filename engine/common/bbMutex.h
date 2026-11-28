#ifndef BB_MUTEX_H
#define BB_MUTEX_H

#include <pthread.h>

#define bbMutexLock(mutex) do { \
    pthread_mutex_lock(mutex); \
} while (0)

#define bbMutexUnlock(mutex) do { \
    pthread_mutex_unlock(mutex); \
} while (0)

#endif