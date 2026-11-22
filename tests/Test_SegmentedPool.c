
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "engine/common/bbHandle.h"
#include "engine/common/bbIntTypes.h"
#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedPool.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"
typedef struct {
    I32 integer;
} test_struct;


// Test basic allocation, handles and alignment.
static void test_allocation(void) {

    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[4];
    test_struct* objects[4];

    for (I32 i = 0; i < 4; i++) {

        bbAssert(
            bbSegmentedPool_alloc(
                pool, &handles[i], (void**)&objects[i]
            ) == bbSuccess,
            "Allocation failed\n"
        );

        bbAssert(objects[i] != NULL,
                 "Null object pointer\n");

        bbAssert(handles[i].system.index == i,
                 "Unexpected handle index\n");

        bbAssert(handles[i].system.generation == 1,
                 "Unexpected generation\n");

        bbAssert((uintptr_t)objects[i] % 16 == 0,
                 "Bad alignment\n");

        objects[i]->integer = 1000 + i;
    }

    for (I32 i = 0; i < 4; i++) {
        bbAssert(objects[i]->integer == 1000 + i,
                 "Object data corrupted\n");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    bbAssert(pool == NULL,
             "Pool pointer not cleared\n");

    printf("test_allocation passed\n");
}


// Test generation checking after slot reuse.
static void test_stale_handle(void) {

    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle old_handle;
    bbHandle new_handle;

    bbAssert(
        bbSegmentedPool_alloc(pool, &old_handle, NULL)
            == bbSuccess,
        "Allocation failed\n"
    );

    bbAssert(
        bbSegmentedPool_free(pool, old_handle) == bbSuccess,
        "Free failed\n"
    );

    bbAssert(
        bbSegmentedPool_alloc(pool, &new_handle, NULL)
            == bbSuccess,
        "Reallocation failed\n"
    );

    bbAssert(
        new_handle.system.index == old_handle.system.index,
        "Freed slot was not reused\n"
    );

    bbAssert(
        new_handle.system.generation != old_handle.system.generation,
        "Generation was not updated\n"
    );

    bbAssert(
        bbSegmentedPool_free(pool, old_handle)
            == bbWrongGeneration,
        "Stale handle not detected\n"
    );

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    printf("test_stale_handle passed\n");
}


// Test double-free detection.
// Run only when warnings are configured as nonfatal.
static void test_double_free(void) {

    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;
    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handle;

    bbAssert(
        bbSegmentedPool_alloc(pool, &handle, NULL)
            == bbSuccess,
        "Allocation failed\n"
    );

    bbAssert(
        bbSegmentedPool_free(pool, handle) == bbSuccess,
        "First free failed\n"
    );

    bbAssert(
        bbSegmentedPool_free(pool, handle) == bbDoubleFree,
        "Double free not detected\n"
    );

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_double_free passed\n");
}


// Test reuse of slots in three different segments.
static void test_available_segments(void) {

    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[16];
    test_struct* objects[16];

    // Fill four segments.
    for (I32 i = 0; i < 16; i++) {

        bbAssert(
            bbSegmentedPool_alloc(
                pool, &handles[i], (void**)&objects[i]
            ) == bbSuccess,
            "Allocation failed\n"
        );
    }

    bbAssert(pool->segment_count == 4,
             "Expected four segments\n");

    // Free slots in three different segments.
    const U32 freed[] = {2, 6, 10};

    for (U32 i = 0; i < 3; i++) {
        bbAssert(
            bbSegmentedPool_free(pool, handles[freed[i]])
                == bbSuccess,
            "Free failed\n"
        );
    }

    // Reallocate from the available-segment list.
    void* reused[3];

    for (U32 i = 0; i < 3; i++) {
        bbAssert(
            bbSegmentedPool_alloc(pool, NULL, &reused[i])
                == bbSuccess,
            "Reallocation failed\n"
        );
    }

    bbAssert(reused[0] == objects[10],
             "Incorrect first reused slot\n");

    bbAssert(reused[1] == objects[6],
             "Incorrect second reused slot\n");

    bbAssert(reused[2] == objects[2],
             "Incorrect third reused slot\n");

    bbAssert(pool->segment_count == 4,
             "Unexpected segment allocation\n");

    bbAssert(
        pool->first_available_segment == BB_POOL_END_OF_LIST,
        "Available segment list should be empty\n"
    );

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    printf("test_available_segments passed\n");
}

static void test_segment_growth(void) {
    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[36];
    test_struct* objects[36];

    for (I32 i = 0; i < 36; i++) {
        bbAssert(
        bbSegmentedPool_alloc(pool, &handles[i], (void**)&objects[i]) == bbSuccess,
        "Allocation failed\n");
        (objects[i])->integer = 1000+i;
    }

    for (I32 i = 0; i < 36; i++) {

        bbAssert((objects[i])->integer == 1000+i,
            "Object data corrupted after segment growth");
    }

    bbAssert(pool->segment_count == 9,
        "Expected nine segments\n");

    bbAssert(pool->segment_capacity >= 9,
        "Segment pointer array did not grow\n");

    bbAssert(pool->first_available_segment == BB_POOL_END_OF_LIST,
        "Available segment list should be empty\n");

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    printf("test_segment_growth passed\n");

}

static void test_segment_lookup(void) {
    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[36];
    test_struct* objects[36];

    for (I32 i = 0; i < 36; i++) {
        bbAssert(
        bbSegmentedPool_alloc(pool, &handles[i], (void**)&objects[i]) == bbSuccess,
        "Allocation failed\n");
        (objects[i])->integer = 1000+i;
    }

    for (I32 i = 0; i < 36; i++) {
        test_struct* object;
        bbAssert(bbSegmentedPool_lookup(pool, handles[i], (void**)&object) == bbSuccess,
            "Lookup failed\n");
        bbAssert(object->integer == 1000+i,"Incorrect object data\n");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");

    printf("test_segement_lookup passed\n");
}

static void test_invalid_lookup(void) {
    bbSegmentedPool* pool = NULL;
    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[36];
    test_struct* objects[36];

    for (I32 i = 0; i < 36; i++) {
        bbAssert(
        bbSegmentedPool_alloc(pool, &handles[i], (void**)&objects[i]) == bbSuccess,
        "Allocation failed\n");
        (objects[i])->integer = 1000+i;
    }

    test_struct* object;

    bbHandle wrong_generation = handles[7];
    wrong_generation.system.generation++;
    bbFlag flag = bbSegmentedPool_lookup(pool, wrong_generation, (void**)&object);
    bbAssert(flag == bbWrongGeneration, "Expected bbWrongGeneration\n");
    bbAssert(object == NULL, "Expected to return NULL\n");

    bbHandle wrong_system = handles[14];
    wrong_system.system.system++;
    flag = bbSegmentedPool_lookup(pool, wrong_system, (void**)&object);
    bbAssert(flag == bbWrongSystem, "Expected bbWrongSystem\n");
    bbAssert(object == NULL, "Expected to return NULL\n");

    bbHandle use_after_free = handles[17];
    flag = bbSegmentedPool_free(pool, use_after_free);
    bbAssert(flag == bbSuccess, "bbSegmentedPool_free failed\n");
    flag = bbSegmentedPool_lookup(pool, use_after_free, (void**)&object);


    bbAssert(flag == bbWrongGeneration, "lookup after free returns bbWrongGeneration?\n");
    bbAssert(object == NULL, "Expected to return NULL\n");


    bbHandle out_of_bounds = handles[21];
    out_of_bounds.system.index = 1000;
    flag = bbSegmentedPool_lookup(pool, out_of_bounds, (void**)&object);
    bbAssert(flag == bbOutOfBounds, "Should return bbOutOfBounds\n");
    bbAssert(object == NULL, "Expected to return NULL\n");

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");
    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_invalid_lookup passed\n");
}


static void test_stress(void) {
    bbSegmentedPool* pool = NULL;
    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, NULL, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handles[10000];
    test_struct* objects[10000];

    for (I32 i = 0; i < 10000; i++) {
        bbAssert(
        bbSegmentedPool_alloc(pool, &handles[i], (void**)&objects[i]) == bbSuccess,
        "Allocation failed\n");
        (objects[i])->integer = i;
    }

    U32 original_segment_count = pool->segment_count;
    test_struct* object;

    for (I32 i = 0; i < 10000; i++) {
        bbAssert(
        bbSegmentedPool_lookup(pool,handles[i],(void**)&object) == bbSuccess,
        "Lookup failed\n");

        bbAssert(object == objects[i], "lookup returned wrong address\n");
        bbAssert(object->integer == i, "lookup returned object with wrong data\n");
    }
    for (I32 i = 0; i < 10000; i+=2) {
        bbFlag flag = bbSegmentedPool_free(pool,handles[i]);
        bbAssert(flag == bbSuccess, "Free failed\n");
    }

    bbAssert(original_segment_count == pool->segment_count,
        "Unexpected increase in segment count\n");

    for (I32 i = 0; i < 10000; i+=2) {
        bbAssert(
        bbSegmentedPool_alloc(pool, &handles[i], (void**)&objects[i]) == bbSuccess,
        "Allocation failed\n");
        (objects[i])->integer = i;
    }
    for (I32 i = 0; i < 10000; i++) {
        bbAssert(
        bbSegmentedPool_lookup(pool,handles[i],(void**)&object) == bbSuccess,
        "Lookup failed\n");

        bbAssert(object == objects[i], "lookup returned wrong address\n");
        bbAssert(object->integer == i, "lookup returned object with wrong data\n");
    }

    bbAssert(pool->first_available_segment == BB_POOL_END_OF_LIST,
        "Available segment list should be empty\n");

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");
    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_stress passed\n");
}

static bbFlag test_init_failure_fn(
    bbSegmentedPool* pool,
    void* object,
    void* cl
) {
    (void)pool;
    (void)object;
    (void)cl;

    return bbFail;
}

static void test_init_failure(void) {

    bbSegmentedPool* pool = NULL;
    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, test_init_failure_fn, NULL)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handle;
    test_struct* object;

    for (I32 i = 0; i < 100; i++) {
        bbFlag flag = bbSegmentedPool_alloc(pool, &handle, (void**)&object);

        bbAssert(flag == bbFail,
            "Expected callback failure\n");
        bbAssert(object == NULL,
            "bbSegmentedPool_alloc returns NULL on failure\n");

        bbAssert(pool->segment_count == 1,
            "Failed allocation caused segment growth");
    }

    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");
    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_init_failure passed\n");
}

static bbFlag test_init_success_fn(
    bbSegmentedPool* pool,
    void* object,
    void* cl
) {

    (void)pool;
    I32* count = cl;
    (*count)++;
    test_struct* test = object;
    test->integer = 193;

    return bbSuccess;
}


static void test_init_success(void) {

    bbSegmentedPool* pool = NULL;
    bool warnings_were_fatal = bbWarningsAreFatal;
    bbWarningsAreFatal = 0;

    I32 callback_count = 0;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0, test_init_success_fn, &callback_count)
            == bbSuccess && pool != NULL,
        "Pool creation failed\n"
    );

    bbHandle handle;
    test_struct* object;

    for (I32 i = 0; i < 100; i++) {
        bbFlag flag = bbSegmentedPool_alloc(pool, &handle, (void**)&object);

        bbAssert(flag == bbSuccess,
            "Allcation failed\n");

        bbAssert(object->integer == 193, "Initialisation failed\n");

    }

    bbAssert(callback_count == 100,
        "Initialisation callback not called 100 times\n");
    bbAssert(bbSegmentedPool_delete(&pool) == bbSuccess,
             "Pool deletion failed\n");
    bbWarningsAreFatal = warnings_were_fatal;
    printf("test_init_success passed\n");
}

int main(void) {

    test_allocation();
    test_stale_handle();
    test_available_segments();
    test_segment_growth();
    test_double_free();
    test_segment_lookup();
    test_invalid_lookup();
    test_stress();
    test_init_failure();
    test_init_success();
    printf("All enabled segmented pool tests passed!\n");

    return 0;
}
