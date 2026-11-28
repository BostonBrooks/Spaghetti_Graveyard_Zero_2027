#ifndef BB_THREADEDEQUE_MACROS_H
#define BB_THREADEDEQUE_MACROS_H
#include <bits/pthreadtypes.h>
#include "engine/storage/deque/bbSegmentedDeque_macros.h"
#include "engine/common/bbFlag.h"

#ifndef CONCAT1
#define CONCAT1(x,y) x##y
#define CONCAT2(x,y) CONCAT1(x,y)
#endif


typedef struct
{
    char string[128];
} bbDummyMessage;


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
CONCAT2(BB_NAME,_deque)* deque, bbDummyMessage* element);\


DECLARE_THREADED_DEQUE_HEADER(name,bbDummyMessage,64)
#endif //BB_THREADEDEQUE_MACROS_H