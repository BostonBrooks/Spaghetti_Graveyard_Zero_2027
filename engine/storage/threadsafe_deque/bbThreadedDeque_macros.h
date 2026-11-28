#ifndef BB_THREADEDEQUE_MACROS_H
#define BB_THREADEDEQUE_MACROS_H
#include <bits/pthreadtypes.h>
#include "engine/storage/deque/bbSegmentedDeque_macros.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbMutex.h"
#include "engine/common/bbMacros.h"




#define DECLARE_THREADED_DEQUE_HEADER(BB_NAME,BB_TYPE,BB_SEGMENT_SIZE) \
\
DECLARE_DEQUE_HEADER(CONCAT2(BB_NAME,_internal),BB_TYPE,BB_SEGMENT_SIZE)\
\
typedef struct CONCAT2(BB_NAME,_deque) {\
    CONCAT2(BB_NAME,_internal_deque) deque;\
    pthread_mutex_t mutex;\
    pthread_cond_t not_empty;\
}CONCAT2(BB_NAME,_deque);\
\
bbFlag CONCAT2(BB_NAME,_dequeInit)\
(CONCAT2(BB_NAME,_deque)* deque);\
bbFlag CONCAT2(BB_NAME,_dequeDestroy)\
(CONCAT2(BB_NAME,_deque)* deque);\
\
bbFlag CONCAT2(BB_NAME,_dequePushFront)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\
\
bbFlag CONCAT2(BB_NAME,_dequePushBack)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\
\
bbFlag CONCAT2(BB_NAME,_dequePopFront)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\
\
bbFlag CONCAT2(BB_NAME,_dequePopBack)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\
\
bbFlag CONCAT2(BB_NAME,_dequeBlockingPopFront)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\
\
bbFlag CONCAT2(BB_NAME,_dequeBlockingPopBack)(\
CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element);\


#define DECLARE_THREADED_DEQUE_SOURCE(BB_NAME,BB_TYPE,BB_SEGMENT_SIZE)\
    \
DECLARE_DEQUE_SOURCE(CONCAT2(BB_NAME,_internal),BB_TYPE,BB_SEGMENT_SIZE)\
\
bbFlag CONCAT2(BB_NAME,_dequeInit)\
(CONCAT2(BB_NAME,_deque)* deque) {\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequeInit)(&deque->deque);\
    if (flag != bbSuccess) return flag;\
\
    int flag2;\
\
    flag2 = pthread_mutex_init(&deque->mutex, NULL);\
    bbAssert(flag2 == 0, "pthread_mutex_init failed\n");\
    flag2 = pthread_cond_init(&deque->not_empty, NULL);\
    bbAssert(flag2 == 0, "pthread_cond_init failed\n");\
\
    return bbSuccess;\
}\
\
\
bbFlag CONCAT2(BB_NAME,_dequeDestroy)(CONCAT2(BB_NAME,_deque)* deque) {\
    bbNotImplemented()\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequePushFront)\
    (CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element)\
{\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePushFront)(&deque->deque, element);\
    if (flag == bbSuccess) pthread_cond_signal(&deque->not_empty);\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequePushBack)(\
    CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element)\
{\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePushBack)(&deque->deque, element);\
    if (flag == bbSuccess) pthread_cond_signal(&deque->not_empty);\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequePopFront)(\
    CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element) {\
\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePopFront)(&deque->deque, element);\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequePopBack)(\
    CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element) {\
\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePopBack)(&deque->deque, element);\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequeBlockingPopFront)(\
    CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element) {\
\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePopFront)(&deque->deque, element);\
    while(flag == bbEmpty) {\
\
        pthread_cond_wait(&deque->not_empty, &deque->mutex);\
        flag = CONCAT2(BB_NAME,_internal_dequePopFront)(&deque->deque, element);\
    }\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequeBlockingPopBack)(\
    CONCAT2(BB_NAME,_deque)* deque, BB_TYPE* element) {\
\
    bbMutexLock(&deque->mutex);\
    bbFlag flag = CONCAT2(BB_NAME,_internal_dequePopBack)(&deque->deque, element);\
    while (flag == bbEmpty) {\
\
        pthread_cond_wait(&deque->not_empty, &deque->mutex);\
        flag = CONCAT2(BB_NAME,_internal_dequePopBack)(&deque->deque, element);\
    }\
    bbMutexUnlock(&deque->mutex);\
    return flag;\
}\


#endif //BB_THREADEDEQUE_MACROS_H