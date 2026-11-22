#include <stdio.h>
#include <stdlib.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedList.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"

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

static void test_init_success(void) {
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

    test_struct* test_object = NULL;
    bbHandle test_handle;
    flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);

    bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");

    test_object->integer = 1;

    flag = bbSegmentedList_pushR(&list,test_object);
    bbAssert(flag == bbSuccess , "pushR failed\n");

    test_struct* test_object2 = NULL;
    flag = bbSegmentedList_popL(&list,(void**)&test_object2);
    bbAssert(flag == bbSuccess && test_object2 != NULL, "popL failed\n");

    bbAssert(test_object2 == test_object,"popL returned wrong object address");
    bbAssert(test_object2->integer == 1, "object data corrupted\n");
    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
         "Pool deletion failed\n");
    bbWarningsAreFatal = warnings_were_fatal;

    printf("test_init_success passed\n");
}


static void test_popL(void) {
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

    test_struct* test_object = NULL;
    bbHandle test_handle;
    for (I32 i = 0; i < 100; i++) {
        flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
        bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
        test_object->integer = i;
        flag = bbSegmentedList_pushR(&list,test_object);
        bbAssert(flag == bbSuccess, "pushR failed\n");
    }


    for (I32 i = 0; i < 50; i++) {
        flag = bbSegmentedList_popL(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
        bbSegmentedPool_freeObject(pool,test_object);

    }

    for (I32 i = 100; i < 150; i++) {
        flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
        bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
        test_object->integer = i;
        flag = bbSegmentedList_pushR(&list,test_object);
        bbAssert(flag == bbSuccess, "pushR failed\n");
    }

    for (I32 i = 50; i < 150; i++) {
        flag = bbSegmentedList_popL(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
        bbSegmentedPool_freeObject(pool,test_object);

    }

    bbAssert(isNULL(list.list_pointer->head),
         "List head should be null\n");

    bbAssert(isNULL(list.list_pointer->tail),
             "List tail should be null\n");

    bbAssert(pool->segment_count == 25,
         "Unexpected segment count\n");

    for (U32 i = 0; i < pool->segment_count; i++) {
        bbAssert(pool->segments[i]->free_count == 4,
                 "Segment not completely free\n");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;

    printf("test_popL passed\n");
}


static void test_popR(void) {
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

    test_struct* test_object = NULL;
    bbHandle test_handle;
    for (I32 i = 0; i < 100; i++) {
        flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
        bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
        test_object->integer = i;
        flag = bbSegmentedList_pushL(&list,test_object);
        bbAssert(flag == bbSuccess, "pushL failed\n");
    }


    for (I32 i = 0; i < 50; i++) {
        flag = bbSegmentedList_popR(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popR failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
        bbSegmentedPool_freeObject(pool,test_object);

    }

    for (I32 i = 100; i < 150; i++) {
        flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
        bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
        test_object->integer = i;
        flag = bbSegmentedList_pushL(&list,test_object);
        bbAssert(flag == bbSuccess, "pushL failed\n");
    }

    for (I32 i = 50; i < 150; i++) {
        flag = bbSegmentedList_popR(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popR failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
        bbSegmentedPool_freeObject(pool,test_object);

    }

    bbAssert(isNULL(list.list_pointer->head),
         "List head should be null\n");

    bbAssert(isNULL(list.list_pointer->tail),
             "List tail should be null\n");

    bbAssert(pool->segment_count == 25,
         "Unexpected segment count\n");

    for (U32 i = 0; i < pool->segment_count; i++) {
        bbAssert(pool->segments[i]->free_count == 4,
                 "Segment not completely free\n");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;

    printf("test_popR passed\n");
}


static void test_list_mixed(void) {
    bbSegmentedPool* pool = NULL;
    bbSegmentedList list;
    bool warnings_were_fatal = bbWarningsAreFatal;
    //bbWarningsAreFatal = 0;


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

    test_struct* test_object = NULL;
    bbHandle test_handle;


    flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
    bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
    test_object->integer = 1;
    flag = bbSegmentedList_pushR(&list,test_object);
    bbAssert(flag == bbSuccess, "pushR failed\n");

    flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
    bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
    test_object->integer = 2;
    flag = bbSegmentedList_pushR(&list,test_object);
    bbAssert(flag == bbSuccess, "pushR failed\n");

    flag = bbSegmentedPool_alloc(pool,&test_handle,(void**)&test_object);
    bbAssert(flag == bbSuccess && test_object != NULL, "Allocation failed\n");
    test_object->integer = 3;
    flag = bbSegmentedList_pushL(&list,test_object);
    bbAssert(flag == bbSuccess, "pushL failed\n");


    flag = bbSegmentedList_popR(&list,(void**)&test_object);
    bbAssert(flag == bbSuccess, "popR failed\n");
    bbAssert(test_object->integer == 2, "Incorrect test_struct data\n");
    bbSegmentedPool_freeObject(pool,test_object);

    bbList_header* header = list.list_pointer;

    test_struct* head_object;
    test_struct* tail_object;

    bbFlag f1 = bbSegmentedPool_lookup(
        pool, header->head, (void**)&head_object);

    bbFlag f2 = bbSegmentedPool_lookup(
        pool, header->tail, (void**)&tail_object);

    bbAssert(f1 == bbSuccess && f2 == bbSuccess,
             "Invalid head or tail\n");

    bbList_element* head_link =
        (bbList_element*)((U8*)head_object + list.offset_of);

    bbList_element* tail_link =
        (bbList_element*)((U8*)tail_object + list.offset_of);

    flag = bbSegmentedList_popL(&list,(void**)&test_object);

    bbAssert(flag == bbSuccess, "popL failed\n");
    bbAssert(test_object->integer == 3, "Incorrect test_struct data\n");
    bbSegmentedPool_freeObject(pool,test_object);

    flag = bbSegmentedList_popR(&list,(void**)&test_object);
    bbAssert(flag == bbSuccess, "popR failed\n");
    bbAssert(test_object->integer == 1, "Incorrect test_struct data\n");
    bbSegmentedPool_freeObject(pool,test_object);




    bbAssert(isNULL(list.list_pointer->head),
         "List head should be null\n");

    bbAssert(isNULL(list.list_pointer->tail),
             "List tail should be null\n");


    for (U32 i = 0; i < pool->segment_count; i++) {
        bbAssert(pool->segments[i]->free_count == 4,
                 "Segment not completely free\n");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;

    printf("test_list_mixed passed\n");
}

int main() {
    test_init_success();
    test_popL();
    test_popR();
    test_list_mixed();

    printf("All enabled segmented list tests passed!\n");
    exit(0);
}