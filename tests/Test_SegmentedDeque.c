#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/deque/bbSegmentedDeque.h"

static void test_deque_push(void){
    bbSegmentedDeque deque;

    bbSegmentedDeque_init(&deque);

    bbDummyStruct dummy;

    for (U32 i = 0; i < 100; i++) {
        sprintf(dummy.data,"pushFront %d\n",i);
        bbSegmentedDeque_pushFront(&deque, &dummy);

        sprintf(dummy.data,"pushBack %d\n",i);
        bbSegmentedDeque_pushBack(&deque, &dummy);
    }

    bbAssert(deque.count == 200,
        "deque.count != the number of elements pushed to the stack\n");

    printf("test_deque_push passed\n");

}

int main() {
    test_deque_push();

    printf("All enabled segmented deque tests passed!\n");
    exit(0);
}