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


static void test_list_remove(void) {
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
    bbHandle test_handles[7];

    for (I32 i = 0; i < 7; i++) {

        flag = bbSegmentedPool_alloc(pool,&test_handles[i],(void**)&test_objects[i]);
        bbAssert(flag == bbSuccess && test_objects[i] != NULL, "Allocation failed\n");
        test_objects[i]->integer = i;
        flag = bbSegmentedList_pushR(&list,(void*)test_objects[i]);
        bbAssert(flag == bbSuccess, "pushR failed\n");
    }

    flag = bbSegmentedList_remove(&list,test_objects[3]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[3]);
    bbAssert(flag == bbSuccess, "free object failed\n");


    flag = bbSegmentedList_remove(&list,test_objects[0]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[0]);
    bbAssert(flag == bbSuccess, "free object failed\n");

    flag = bbSegmentedList_remove(&list,test_objects[6]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[6]);
    bbAssert(flag == bbSuccess, "free object failed\n");

    flag = bbSegmentedList_remove(&list,test_objects[4]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[4]);
    bbAssert(flag == bbSuccess, "free object failed\n");

    flag = bbSegmentedList_remove(&list,test_objects[5]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[5]);
    bbAssert(flag == bbSuccess, "free object failed\n");

    flag = bbSegmentedList_remove(&list,test_objects[1]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[1]);
    bbAssert(flag == bbSuccess, "free object failed\n");

    flag = bbSegmentedList_remove(&list,test_objects[2]);
    bbAssert(flag == bbSuccess, "remove failed\n");
    flag = bbSegmentedPool_freeObject(pool,test_objects[2]);
    bbAssert(flag == bbSuccess, "free object failed\n");

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

    printf("test_list_remove passed\n");
}

static void test_list_insert(void) {
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
    bbHandle test_handles[7];

    for (I32 i = 0; i < 7; i++) {

        flag = bbSegmentedPool_alloc(pool,&test_handles[i],(void**)&test_objects[i]);
        bbAssert(flag == bbSuccess && test_objects[i] != NULL, "Allocation failed\n");
        test_objects[i]->integer = i;
    }


    flag = bbSegmentedList_pushL(&list,(void*)test_objects[0]);
    bbAssert(flag == bbSuccess, "pushL failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[0],test_objects[3]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[0],test_objects[1]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[3],test_objects[5]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[1],test_objects[2]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[5],test_objects[6]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    bbSegmentedList_insertAfter(&list, test_objects[3],test_objects[4]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");


    test_struct* test_object = NULL;
    for (I32 i = 0; i < 7; i++) {
        flag = bbSegmentedList_popL(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
    }

    flag = bbSegmentedList_pushR(&list,test_objects[6]);
    bbAssert(flag == bbSuccess, "pushR failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[6],test_objects[3]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[3],test_objects[1]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[6],test_objects[5]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[3],test_objects[2]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[5],test_objects[4]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    flag = bbSegmentedList_insertBefore(&list, test_objects[1],test_objects[0]);
    bbAssert(flag == bbSuccess, "insertAfter failed\n");

    for (I32 i = 0; i < 7; i++) {
        flag = bbSegmentedList_popL(&list,(void**)&test_object);
        bbAssert(flag == bbSuccess, "popL failed\n");
        bbAssert(test_object->integer == i, "Incorrect test_struct data\n");
        flag = bbSegmentedPool_freeObject(pool,test_object);
        bbAssert(flag == bbSuccess, "freeObject failed\n");
    }


    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_list_insert passed\n");
}


static void test_list_getNext(void) {
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
    bbHandle test_handles[7];

    for (I32 i = 0; i < 7; i++) {

        flag = bbSegmentedPool_alloc(pool,&test_handles[i],(void**)&test_objects[i]);
        bbAssert(flag == bbSuccess && test_objects[i] != NULL, "Allocation failed\n");
        test_objects[i]->integer = i;
        flag = bbSegmentedList_pushR(&list,(void*)test_objects[i]);
        bbAssert(flag == bbSuccess, "pushR failed\n");
    }

    test_struct* test_object = test_objects[2];
    flag = bbSegmentedList_getNext(&list,test_objects[0],(void**)&test_object);
    bbAssert(test_object != NULL && flag == bbSuccess, "getNext failed\n");
    bbAssert(test_object->integer == 1, "getNext returned incorrect data\n");

    flag = bbSegmentedList_getPrev(&list,test_object,(void**)&test_object);
    bbAssert(test_object != NULL && flag == bbSuccess, "getPrev failed\n");
    bbAssert(test_object->integer == 0, "getPrev returned incorrect data\n");

    flag = bbSegmentedList_getNext(
        &list, test_objects[3], (void**)&test_object);

    bbAssert(flag == bbSuccess &&
             test_object == test_objects[4],
             "getNext across segment boundary failed\n");

    flag = bbSegmentedList_getPrev(
        &list, test_object, (void**)&test_object);

    bbAssert(flag == bbSuccess &&
             test_object == test_objects[3],
             "getPrev across segment boundary failed\n");

    flag = bbSegmentedList_getNext(
        &list, test_objects[6], (void**)&test_object);

    bbAssert(flag == bbEmpty, "Expected bbEmpty\n");

    flag = bbSegmentedList_getPrev(
        &list, test_objects[0], (void**)&test_object);

    bbAssert(flag == bbEmpty, "Expected bbEmpty\n");

    test_struct* foreign_object = NULL;
    flag = bbSegmentedPool_alloc(pool,NULL,(void**)&foreign_object);
    bbAssert(flag == bbSuccess && foreign_object != NULL, "Allocation failed\n");


    flag = bbSegmentedList_getPrev(
        &list, foreign_object, (void**)&test_object);
    bbAssert(flag == bbFail, "bbSegmentedList_getPrev() Expected bbFail\n");

    flag = bbSegmentedList_getNext(
        &list, foreign_object, (void**)&test_object);
    bbAssert(flag == bbFail, "bbSegmentedList_getNext() Expected bbFail\n");


    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
     "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_list_getNext passed\n");
}

int main() {

    debug_off = false;
    test_init_success();
    test_popL();
    test_popR();
    test_list_mixed();
    test_list_remove();
    test_list_insert();
    test_list_getNext();
    printf("All enabled segmented list tests passed!\n");
    exit(0);
}