#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/deque/bbSegmentedDeque.h"
#include "engine/storage/deque/bbSegmentedDeque_iterator.h"
#include "engine/storage/deque/bbSegmentedDeque_macros.h"
#include "engine/storage/deque/bbSegmentedDeque_iteratorMacros.h"
static void test_deque_iterator(void){
    bbSegmentedDeque deque;

    bbSegmentedDeque_init(&deque);

    bbDummyStruct payload;
    bbFlag flag;

    for (U32 i = 0; i < 20; i++) {
        sprintf((char*)payload.data,"test_payload %u",i);
        flag = bbSegmentedDeque_pushFront(&deque, &payload);
        bbAssert(flag == bbSuccess,"_pushFront failed\n");
    }

    const void* data;

    char expected_output[128];
    bbSegmentedDeque_iterator iterator;

    flag = bbSegmentedDeque_iteratorFront(&deque, &iterator, &data);

    for (U32 i = 0; i < 20; i++) {
        bbAssert(flag == bbSuccess,"iterator returned unexpected value\n");
        const bbDummyStruct* payload_out = data;
        sprintf(expected_output,"test_payload %u",19-i);
        bbAssert(strcmp(expected_output,(char*)payload_out->data) == 0,"iterator returned bad data\n");
        flag = bbSegmentedDeque_iteratorNext(&iterator, &data);
    }
    bbAssert(flag == bbEmpty,"_iteratorNext should return bbEmpty\n");
    flag = bbSegmentedDeque_iteratorGet(&iterator, &data);
    for (U32 i = 0; i < 20; i++) {
        bbAssert(flag == bbSuccess,"iterator returned unexpected value\n");
        const bbDummyStruct* payload_out = data;
        sprintf(expected_output,"test_payload %u",i);
        bbAssert(strcmp(expected_output,(char*)payload_out->data) == 0,"iterator returned bad data\n");

        flag = bbSegmentedDeque_iteratorPrev(&iterator, &data);
    }

    bbAssert(flag == bbEmpty,"_iteratorNext should return bbEmpty\n");



    bbSegmentedDeque_destroy(&deque);
    printf("test_deque_iterator passed\n");

}


DECLARE_DEQUE_HEADER(bbName,bbDummyStruct, 4)
DECLARE_DEQUE_ITERATOR_HEADER(bbName,bbDummyStruct, 4)

static void test_deque_iterator_macros(void){
    bbName_deque deque;

    bbName_dequeInit(&deque);

    bbDummyStruct payload;
    bbFlag flag;

    for (U32 i = 0; i < 20; i++) {
        sprintf((char*)payload.data,"test_payload %u",i);
        flag = bbName_dequePushFront(&deque, &payload);
        bbAssert(flag == bbSuccess,"_pushFront failed\n");
    }

    const bbDummyStruct* data;

    char expected_output[128];
    bbName_dequeIterator iterator;

    flag = bbName_dequeIteratorFront(&deque, &iterator, &data);

    for (U32 i = 0; i < 20; i++) {
        bbAssert(flag == bbSuccess,"iterator returned unexpected value\n");
        const bbDummyStruct* payload_out = data;
        sprintf(expected_output,"test_payload %u",19-i);
        bbAssert(strcmp(expected_output,(char*)payload_out->data) == 0,
            "iterator returned bad data %s, %s\n", expected_output, (char*)payload_out->data);
        flag = bbName_dequeIteratorNext(&iterator, &data);
    }
    bbAssert(flag == bbEmpty,"_iteratorNext should return bbEmpty\n");
    flag = bbName_dequeIteratorGet(&iterator, &data);
    for (U32 i = 0; i < 20; i++) {
        bbAssert(flag == bbSuccess,"iterator returned unexpected value\n");
        const bbDummyStruct* payload_out = data;
        sprintf(expected_output,"test_payload %u",i);
        bbAssert(strcmp(expected_output,(char*)payload_out->data) == 0,"iterator returned bad data\n");

        flag = bbName_dequeIteratorPrev(&iterator, &data);
    }

    bbAssert(flag == bbEmpty,"_iteratorNext should return bbEmpty\n");



    bbName_dequeDestroy(&deque);
    printf("test_deque_iterator_macros passed\n");

}

int main() {
    test_deque_iterator();
    test_deque_iterator_macros();

    printf("All enabled segmented deque iterator tests passed!\n");
    exit(0);
}



DECLARE_DEQUE_SOURCE(bbName,bbDummyStruct, 4)
DECLARE_DEQUE_ITERATOR_SOURCE(bbName,bbDummyStruct, 4)