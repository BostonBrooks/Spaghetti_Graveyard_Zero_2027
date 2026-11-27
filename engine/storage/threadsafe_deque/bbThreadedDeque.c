#include <pthread.h>
#include "engine/storage/threadsafe_deque/bbThreadedDeque.h"

#include <stdlib.h>

#include "engine/common/bbTerminal.h"


DECLARE_DEQUE_SOURCE(bbThreaded,bbDummyMessage,1024)

#define bbMutexLock(mutex){\
pthread_mutex_lock(mutex);}\

#define bbMutexUnlock(mutex){\
pthread_mutex_unlock(mutex);}\

bbFlag bbThreadedDeque_init(bbThreadedDeque* deque) {
    bbFlag flag = bbThreaded_dequeInit(&deque->deque);
    if (flag != bbSuccess) return flag;

    int flag2;

    flag2 = pthread_mutex_init(&deque->mutex, NULL);
    bbAssert(flag2 == 0, "pthread_mutex_init failed\n");
    flag2 = pthread_cond_init(&deque->not_empty, NULL);
    bbAssert(flag2 == 0, "pthread_cond_init failed\n");

    return bbSuccess;
}


bbFlag bbThreadedDeque_pushFront(
    bbThreadedDeque* deque, bbDummyMessage* element)
{
    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePushFront(&deque->deque, element);
    if (flag == bbSuccess) pthread_cond_signal(&deque->not_empty);
    bbMutexUnlock(&deque->mutex);
    return flag;
}


bbFlag bbThreadedDeque_pushBack(
    bbThreadedDeque* deque, bbDummyMessage* element)
{
    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePushBack(&deque->deque, element);
    if (flag == bbSuccess) pthread_cond_signal(&deque->not_empty);
    bbMutexUnlock(&deque->mutex);
    return flag;
}



bbFlag bbThreadedDeque_popFront(
    bbThreadedDeque* deque, bbDummyMessage* element) {

    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePopFront(&deque->deque, element);
    bbMutexUnlock(&deque->mutex);
    return flag;
}

bbFlag bbThreadedDeque_popBack(
    bbThreadedDeque* deque, bbDummyMessage* element) {

    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePopBack(&deque->deque, element);
    bbMutexUnlock(&deque->mutex);
    return flag;
}


bbFlag bbThreadedDeque_blockingPopFront(
    bbThreadedDeque* deque, bbDummyMessage* element) {

    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePopFront(&deque->deque, element);
    while(flag == bbEmpty) {

        pthread_cond_wait(&deque->not_empty, &deque->mutex);
        flag = bbThreaded_dequePopFront(&deque->deque, element);
    }
    bbMutexUnlock(&deque->mutex);
    return flag;
}

bbFlag bbThreadedDeque_blockingPopBack(
bbThreadedDeque* deque, bbDummyMessage* element){

    bbMutexLock(&deque->mutex);
    bbFlag flag = bbThreaded_dequePopBack(&deque->deque, element);
    while (flag == bbEmpty) {

        pthread_cond_wait(&deque->not_empty, &deque->mutex);
        flag = bbThreaded_dequePopBack(&deque->deque, element);
    }
    bbMutexUnlock(&deque->mutex);
    return flag;
}