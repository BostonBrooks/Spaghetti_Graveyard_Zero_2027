#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/deque/bbSegmentedDeque.h"
#include "engine/storage/deque/bbSegmentedDeque_iterator.h"

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


int main() {
    test_deque_iterator();

    printf("All enabled segmented deque iterator tests passed!\n");
    exit(0);
}