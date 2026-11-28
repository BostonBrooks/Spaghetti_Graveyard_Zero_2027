#include <bits/pthreadtypes.h>
#ifndef BB_THREADSAFE_DEQUE_H
#define BB_THREADSAFE_DEQUE_H

#include "engine/storage/deque/bbSegmentedDeque_macros.h"
#include "engine/common/bbMacros.h"

typedef struct
{
    char string[128];
} bbDummyMessage;


DECLARE_DEQUE_HEADER(bbThreaded,bbDummyMessage,1024)

typedef struct bbThreadedDeque {
    bbThreaded_deque deque;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
}bbThreadedDeque;


bbFlag bbThreadedDeque_init(bbThreadedDeque* deque);
bbFlag bbThreadedDeque_destroy(bbThreadedDeque* deque);

bbFlag bbThreadedDeque_pushFront(
bbThreadedDeque* deque, bbDummyMessage* element);

bbFlag bbThreadedDeque_pushBack(
bbThreadedDeque* deque, bbDummyMessage* element);

bbFlag bbThreadedDeque_popFront(
bbThreadedDeque* deque, bbDummyMessage* element);

bbFlag bbThreadedDeque_popBack(
bbThreadedDeque* deque, bbDummyMessage* element);


bbFlag bbThreadedDeque_blockingPopFront(
bbThreadedDeque* deque, bbDummyMessage* element);

bbFlag bbThreadedDeque_blockingPopBack(
bbThreadedDeque* deque, bbDummyMessage* element);

#endif //BB_THREADSAFE_DEQUE_H