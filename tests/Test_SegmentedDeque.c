#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/deque/bbSegmentedDeque.h"

static void test_deque_push(void){
    bbSegmentedDeque deque;

    bbSegmentedDeque_init(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"pushFront %d",i);
        bbSegmentedDeque_pushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d",i);
        bbSegmentedDeque_pushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    bbSegmentedDeque_destroy(&deque);
    printf("test_deque_push passed\n");

}


static void test_deque_pop(void){
    bbSegmentedDeque deque;

    bbSegmentedDeque_init(&deque);

    bbDummyStruct dummy;
    bbFlag flag;
    char expected_data[128];

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");


    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    flag = bbSegmentedDeque_destroy(&deque);
    bbAssert(flag == bbSuccess, "flag should be successful\n");

    printf("test_deque_pop passed\n");

}


int main() {
    test_deque_push();
    test_deque_pop();

    printf("All enabled segmented deque tests passed!\n");
    exit(0);
}