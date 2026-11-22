#ifndef BB_SEGMENTED_POOL_H
#define BB_SEGMENTED_POOL_H
#include <stdbool.h>

#include "engine/common/bbFlag.h"


typedef struct bbSegmentedPool bbSegmentedPool;

typedef bbFlag bbSegmentedPool_initFn(bbSegmentedPool* pool, void* object, void* cl);


static inline bool bbHandle_equal(bbHandle a, bbHandle b) {
    return a.system.index == b.system.index &&
           a.system.generation == b.system.generation &&
           a.system.system == b.system.system;
}

/**
 * @brief Creates an empty segmented memory pool.
 *
 * Segments are allocated lazily.
 *
 * @param[out] out_pool Receives the allocated pool.
 * @param element_size Size of each user element in bytes.
 * @param alignment Required alignment of each element.
 * @param elements_per_segment Number of elements per segment.
 * @param init_fn callback applied to each element on alloc
 * The callback should not itself free the allocated object,
 * but return a failure flag if the initialisation fails
 * @param init_cl data supplied to init_fn
 *
 * @return bbSuccess on success, bbFail on failure.
 */
bbFlag bbSegmentedPool_new(
    bbSegmentedPool** out_pool,
    size_t element_size,
    size_t alignment,
    U32 elements_per_segment,
    U16 system,
    bbSegmentedPool_initFn* init_fn,
    void* init_cl);

bbFlag bbSegmentedPool_alloc(
    bbSegmentedPool* pool,
    bbHandle* out_handle,
    void** out_data
);

bbFlag bbSegmentedPool_free(
    bbSegmentedPool* pool,
    bbHandle handle
);



bbFlag bbSegmentedPool_freeObject(
    bbSegmentedPool* pool,
    void* object
);

bbFlag bbSegmentedPool_lookup(bbSegmentedPool* pool,
    bbHandle handle,
    void** out_data);

/**
 * @brief Finds the handle of an object in a pool
 *
 * @warning passing an invalid object is an unchecked error
 */
bbFlag bbSegmentedPool_reverseLookup(
    bbSegmentedPool* pool,
    void* object,
    bbHandle* out_handle
);

bbFlag bbSegmentedPool_delete(bbSegmentedPool** pool);

#endif //BB_SEGMENTED_POOL_H