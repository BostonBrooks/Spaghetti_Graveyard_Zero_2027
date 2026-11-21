#include <stdlib.h>

#include "engine/common/bbHandle.h"
#include "engine/common/bbIntTypes.h"
#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedPool.h"

typedef struct {
    I32 integer;
} test_struct;

int main() {

    bbSegmentedPool* pool;
    bbFlag result = bbSegmentedPool_new(&pool,sizeof(test_struct),16,1,0);

    bbAssert(result == bbSuccess && pool != NULL, "bbSegmentedPool_new failed\n");
    bbHandle handles[100];
    test_struct* objects[100];


    for (I32 i = 0; i < 100; i++) {
        bbAssert(
            bbSegmentedPool_alloc(pool,&handles[i],(void**)&objects[i]) == bbSuccess,
            "Allocation failed\n"
        );

        bbAssert(objects[i] != NULL, "Allocation failed\n");
        bbAssert(handles[i].system.index == i, "Unexpected handle index\n");
        bbAssert(handles[i].system.generation == 1, "Unexpected generation\n");
        bbAssert((uintptr_t)objects[i] %8 == 0,"Bad alignment\n");
        objects[i]->integer = 1000+i;

        printf("Allocation %2d: index=%u, address = %p\n",
            i,
            handles[i].system.index,
            (void*)objects[i]);
    }
   printf("All 100 allocations passed!\n");

    return 0;
}