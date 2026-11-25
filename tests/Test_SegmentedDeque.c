#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
//#include "engine/storage/deque/bbSegmentedDeque.h"

#include "engine/storage/deque/bbSegmentedDeque2.h"

typedef struct
{
    U8 data[128];
} bbDummyStruct;


DECLARE_DEQUE_HEADER(bbSegmentedDeque2,bbDummyStruct, 1)

static void test_deque_push2(void){
    bbSegmentedDeque2 deque;

    bbSegmentedDeque2_init(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"pushFront %d",i);
        bbSegmentedDeque2_pushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d",i);
        bbSegmentedDeque2_pushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    bbSegmentedDeque2_destroy(&deque);
    printf("test_deque_push passed\n");

}


static void test_deque_pop2(void){
    bbSegmentedDeque2 deque;

    bbSegmentedDeque2_init(&deque);

    bbDummyStruct dummy;
    bbFlag flag;
    char expected_data[128];

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque2_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque2_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque2_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque2_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque2_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");


    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque2_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque2_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque2_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque2_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque2_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    flag = bbSegmentedDeque2_destroy(&deque);
    bbAssert(flag == bbSuccess, "flag should be successful\n");

    printf("test_deque_pop passed\n");

}

DECLARE_DEQUE_HEADER(bbSegmentedDeque3,bbDummyStruct, 4)

static void test_deque_push3(void){
    bbSegmentedDeque3 deque;

    bbSegmentedDeque3_init(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"pushFront %d",i);
        bbSegmentedDeque3_pushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d",i);
        bbSegmentedDeque3_pushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    bbSegmentedDeque3_destroy(&deque);
    printf("test_deque_push passed\n");

}


static void test_deque_pop3(void){
    bbSegmentedDeque3 deque;

    bbSegmentedDeque3_init(&deque);

    bbDummyStruct dummy;
    bbFlag flag;
    char expected_data[128];

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque3_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque3_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque3_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque3_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque3_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");


    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbSegmentedDeque3_pushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbSegmentedDeque3_pushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbSegmentedDeque3_popBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbSegmentedDeque3_popFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbSegmentedDeque3_popFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    flag = bbSegmentedDeque3_destroy(&deque);
    bbAssert(flag == bbSuccess, "flag should be successful\n");

    printf("test_deque_pop passed\n");

}

int main() {
    test_deque_push2();
    test_deque_pop2();
    test_deque_push3();
    test_deque_pop3();

    printf("All enabled segmented deque tests passed!\n");
    exit(0);
}


DECLARE_DEQUE_SOURCE(bbSegmentedDeque2,bbDummyStruct, 1)
DECLARE_DEQUE_SOURCE(bbSegmentedDeque3,bbDummyStruct, 4)