#include <pthread.h>
#include <SFML/System/Sleep.h>

#include "engine/storage/threadsafe_deque/bbThreadedDeque_macros.h"
typedef struct
{
    char string[128];
} bbDummyMessage;

DECLARE_THREADED_DEQUE_HEADER(bbTest,bbDummyMessage,8)

typedef struct test_producer_cl {
    bbTest_deque* deque;
    U32 thread_id;
}test_producer_cl;




void* test_producer_fn(void* arg) {
    test_producer_cl* cl = (test_producer_cl*)arg;

    bbTest_deque* deque = cl->deque;
    U32 thread_id = cl->thread_id;

    bbDummyMessage message;
    for (U32 i = 0; i < 10; i++) {
        sprintf(message.string, "Thread: %u, Message: %u",
            thread_id,i);
        bbTest_dequePushFront(deque, &message);
    }

    return NULL;
}

void* test_producer2_fn(void* arg) {
    test_producer_cl* cl = (test_producer_cl*)arg;

    bbTest_deque* deque = cl->deque;
    U32 thread_id = cl->thread_id;

    bbDummyMessage message;
    for (U32 i = 0; i < 10; i++) {
        sprintf(message.string, "Thread: %u, Message: %u",
            thread_id,i);
        bbTest_dequePushFront(deque, &message);
        sfSleep(sfSeconds(0.1f));
    }

    return NULL;


}

static void test_consumer(void) {
    bbTest_deque deque;
    bbTest_dequeInit(&deque);

    test_producer_cl cl1;
    cl1.deque = &deque;
    cl1.thread_id = 1;
    pthread_t test_producer1;

    test_producer_cl cl2;
    cl2.deque = &deque;
    cl2.thread_id = 2;
    pthread_t test_producer2;

    test_producer_cl cl3;
    cl3.deque = &deque;
    cl3.thread_id = 3;
    pthread_t test_producer3;

    test_producer_cl cl4;
    cl4.deque = &deque;
    cl4.thread_id = 4;
    pthread_t test_producer4;

    int pthread_result;
    pthread_result = pthread_create(&test_producer1, NULL, test_producer_fn, &cl1);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer2, NULL, test_producer_fn, &cl2);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer3, NULL, test_producer_fn, &cl3);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer4, NULL, test_producer_fn, &cl4);
    bbAssert(pthread_result == 0, "pthread_create failed\n");


    bbDummyMessage message;
    bbFlag flag;
    U32 received_message;
    U32 from_thread;
    U32 expected_message[4] = {0};
    for (U32 i = 0; i<40;) {
        flag = bbTest_dequePopBack(&deque, &message);
        if (flag == bbSuccess) {
            sscanf(
                message.string,
                "Thread: %u, Message: %u",
                &from_thread,
                &received_message);
            fflush(stdout);
            bbAssert(received_message == expected_message[from_thread-1],
                "message received in incorrect order %u, %u, %u\n",
                received_message, expected_message[from_thread-1],from_thread);
            expected_message[from_thread-1]++;
            i++;
        } else {
        }
    }
    pthread_join(test_producer1, NULL);
    pthread_join(test_producer2, NULL);
    pthread_join(test_producer3, NULL);
    pthread_join(test_producer4, NULL);

    bbTest_dequeDestroy(&deque);

    printf("test_producer_fn and test_consumer passed\n");

}


static void test_consumer2(void) {
    bbTest_deque deque;
    bbTest_dequeInit(&deque);

    test_producer_cl cl1;
    cl1.deque = &deque;
    cl1.thread_id = 1;
    pthread_t test_producer1;

    test_producer_cl cl2;
    cl2.deque = &deque;
    cl2.thread_id = 2;
    pthread_t test_producer2;

    test_producer_cl cl3;
    cl3.deque = &deque;
    cl3.thread_id = 3;
    pthread_t test_producer3;

    test_producer_cl cl4;
    cl4.deque = &deque;
    cl4.thread_id = 4;
    pthread_t test_producer4;

    int pthread_result;
    pthread_result = pthread_create(&test_producer1, NULL, test_producer2_fn, &cl1);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer2, NULL, test_producer2_fn, &cl2);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer3, NULL, test_producer2_fn, &cl3);
    bbAssert(pthread_result == 0, "pthread_create failed\n");
    pthread_result = pthread_create(&test_producer4, NULL, test_producer2_fn, &cl4);
    bbAssert(pthread_result == 0, "pthread_create failed\n");


    bbDummyMessage message;
    bbFlag flag;
    U32 received_message;
    U32 from_thread;
    U32 expected_message[4] = {0};
    for (U32 i = 0; i<40;) {
        flag = bbTest_dequeBlockingPopBack(&deque, &message);
        if (flag == bbSuccess) {
            sscanf(
                message.string,
                "Thread: %u, Message: %u",
                &from_thread,
                &received_message);
            fflush(stdout);
            bbAssert(received_message == expected_message[from_thread-1],
                "message received in incorrect order %u, %u, %u\n",
                received_message, expected_message[from_thread-1],from_thread);
            expected_message[from_thread-1]++;
            i++;
        } else {
        }
    }
    pthread_join(test_producer1, NULL);
    pthread_join(test_producer2, NULL);
    pthread_join(test_producer3, NULL);
    pthread_join(test_producer4, NULL);

    bbTest_dequeDestroy(&deque);

    printf("test_producer2_fn and test_consumer2 passed\n");

}

int main() {
    test_consumer();
    test_consumer2();

    printf("All enabled threaded deque tests passed!\n");
    return 0;
}


DECLARE_THREADED_DEQUE_SOURCE(bbTest,bbDummyMessage,8)