#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
//#include "engine/storage/deque/bbSegmentedDeque.h"

#include "engine/storage/deque/bbSegmentedDeque_macros.h"

typedef struct
{
    U8 data[128];
} bbDummyStruct;


DECLARE_DEQUE_HEADER(bbDummyStruct,bbDummyStruct, 1)
DECLARE_DEQUE_HEADER(bbDummyStruct2,bbDummyStruct, 4)

static void test_deque_push1(void){
    bbDummyStruct_deque deque;

    bbDummyStruct_dequeInit(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"pushFront %d",i);
        bbDummyStruct_dequePushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d",i);
        bbDummyStruct_dequePushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    bbDummyStruct_dequeDestroy(&deque);
    printf("test_deque_push1 passed\n");

}


static void test_deque_pop1(void){
    bbDummyStruct_deque deque;

    bbDummyStruct_dequeInit(&deque);

    bbDummyStruct dummy;
    bbFlag flag;
    char expected_data[128];

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbDummyStruct_dequePushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbDummyStruct_dequePushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbDummyStruct_dequePopBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbDummyStruct_dequePopFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbDummyStruct_dequePopFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");


    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbDummyStruct_dequePushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbDummyStruct_dequePushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbDummyStruct_dequePopBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbDummyStruct_dequePopFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbDummyStruct_dequePopFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    flag = bbDummyStruct_dequeDestroy(&deque);
    bbAssert(flag == bbSuccess, "flag should be successful\n");

    printf("test_deque_pop1 passed\n");

}


static void test_deque_push2(void){
    bbDummyStruct2_deque deque;

    bbDummyStruct2_dequeInit(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"pushFront %d",i);
        bbDummyStruct2_dequePushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d",i);
        bbDummyStruct2_dequePushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    bbDummyStruct2_dequeDestroy(&deque);
    printf("test_deque_push2 passed\n");

}


static void test_deque_pop2(void){
    bbDummyStruct2_deque deque;

    bbDummyStruct2_dequeInit(&deque);

    bbDummyStruct dummy;
    bbFlag flag;
    char expected_data[128];

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbDummyStruct2_dequePushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbDummyStruct2_dequePushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbDummyStruct2_dequePopBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbDummyStruct2_dequePopFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbDummyStruct2_dequePopFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");


    for (I32 i = 0; i < 10; i++) {
        sprintf(dummy.data,"Front %d",i);
        flag = bbDummyStruct2_dequePushFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");

        sprintf(dummy.data,"Back %d",i);
        flag = bbDummyStruct2_dequePushBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
    }

    bbAssert(deque.count == 20,
        "deque.count != the number of elements pushed to the stack\n");

    for (I32 i = 9; i >= 0; i--) {
        flag = bbDummyStruct2_dequePopBack(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Back %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");
        flag = bbDummyStruct2_dequePopFront(&deque, &dummy);
        bbAssert(flag == bbSuccess, "flag should be successful\n");
        sprintf(expected_data,"Front %d",i);
        bbAssert(strcmp(expected_data,dummy.data) == 0,
            "unexpected data popped from deque\n");

    }

    bbAssert(deque.count == 0,
        "The deque should be empty\n");
    flag = bbDummyStruct2_dequePopFront(&deque, &dummy);
    bbAssert(flag == bbEmpty,"The deque should be empty\n");

    bbAssert(deque.head == NULL && deque.tail == NULL,
        "empty deque should have null free list\n");

    flag = bbDummyStruct2_dequeDestroy(&deque);
    bbAssert(flag == bbSuccess, "flag should be successful\n");

    printf("test_deque_pop2 passed\n");

}

int main() {
    test_deque_push1();
    test_deque_pop1();
    test_deque_push2();
    test_deque_pop2();

    printf("All enabled segmented deque tests passed!\n");
    exit(0);
}


DECLARE_DEQUE_SOURCE(bbDummyStruct,bbDummyStruct, 1)
DECLARE_DEQUE_SOURCE(bbDummyStruct2,bbDummyStruct, 4)