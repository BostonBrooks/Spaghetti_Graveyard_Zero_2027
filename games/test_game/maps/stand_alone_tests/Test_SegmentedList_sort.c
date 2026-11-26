#include <stdio.h>
#include <stdlib.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedList.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"
#include "engine/storage/segmented/bbSegmentedList_sort.h"
#define isNULL(A) ((A).system.generation == BB_POOL_NULL_GENERATION)

typedef struct {
    I32 integer;
    bbList_element element;
} test_struct;

static bbFlag test_init_list_fn(
    bbSegmentedPool* pool,
    void* object,
    void* cl
) {

    (void)pool;
    (void)cl;
    test_struct* test_struct = object;

    test_struct->element.list_id = 0;
    test_struct->element.prev = (bbHandle){0};
    test_struct->element.next = (bbHandle){0};
    return bbSuccess;
}

I32 test_comparator(
    void* object_A,
    void* object_B)
{
    test_struct* test_A = (test_struct*)object_A;
    test_struct* test_B = (test_struct*)object_B;

    if (test_A->integer < test_B->integer) return -1;
    if (test_A->integer == test_B->integer) return 0;
    return 1;
}

static void test_list_sortL(void) {
    bbSegmentedPool* pool = NULL;
    bbSegmentedList list;
    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;


    bbFlag flag =  bbSegmentedPool_new(&pool,
        sizeof(test_struct),
        16,
        4,
        7,
        test_init_list_fn,
        NULL);

    bbAssert(flag == bbSuccess && pool != NULL, "Pool creation failed\n");

    bbSegmentedList_init(&list,
        pool,
        offsetof(test_struct, element),
        193,
        NULL);

    test_struct* test_objects[7];
    test_struct* test_object;
    bbHandle test_handles[7];

    for (I32 i = 0; i < 7; i++) {

        flag = bbSegmentedPool_alloc(pool,&test_handles[i],(void**)&test_objects[i]);
        bbAssert(flag == bbSuccess && test_objects[i] != NULL, "Allocation failed\n");
        test_objects[i]->integer = i;
        bbAssert(flag == bbSuccess, "pushR failed\n");
    }
    flag = bbSegmentedList_sortL(&list, test_comparator,test_objects[3]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortR(&list, test_comparator,test_objects[1]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortL(&list, test_comparator,test_objects[0]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortR(&list, test_comparator,test_objects[6]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortL(&list, test_comparator,test_objects[4]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortR(&list, test_comparator,test_objects[5]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");
    flag = bbSegmentedList_sortL(&list, test_comparator,test_objects[2]);
    bbAssert(flag == bbSuccess, "Sorting failed\n");

    for (I32 i = 0; i < 7; i++) {
        flag = bbSegmentedList_popL(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");

    }


    flag = bbSegmentedList_popL(&list,(void**)&test_object);
    bbAssert(flag == bbEmpty, "popL didn't return bbEmpty: %d\n", flag);

    for (I32 i = 0; i < 7; i++) {
        test_objects[i]->integer = 42;

        if (i % 2 == 0)
            flag = bbSegmentedList_sortL(
                &list, test_comparator, test_objects[i]);
        else
            flag = bbSegmentedList_sortR(
                &list, test_comparator, test_objects[i]);

        bbAssert(flag == bbSuccess, "Sorting failed\n");
    }

    for (I32 i = 0; i < 7; i++) {
        flag = bbSegmentedList_popL(&list, (void**)&test_object);

        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object == test_objects[i],
            "Stable insertion order violated\n");

        flag = bbSegmentedPool_freeObject(list.pool,test_objects[i]);
        bbAssert(flag == bbSuccess, "_freeObject failed\n");
    }



    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_list_getNext passed\n");
}

int main() {

    debug_off = false;
    test_list_sortL();
    printf("All enabled segmented list tests passed!\n");
    exit(0);
}