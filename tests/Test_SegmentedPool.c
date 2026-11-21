
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
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0)
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
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0)
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

    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0)
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

    printf("test_double_free passed\n");
}


// Test reuse of slots in three different segments.
static void test_available_segments(void) {

    bbSegmentedPool* pool = NULL;

    bbAssert(
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0)
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
        bbSegmentedPool_new(&pool, sizeof(test_struct), 16, 4, 0)
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

int main(void) {

    test_allocation();
    test_stale_handle();
    test_available_segments();
    test_segment_growth();
    // Enable only when double-free warnings are nonfatal.
    // test_double_free();

    printf("All enabled segmented pool tests passed!\n");

    return 0;
}
