#ifndef BB_SEGMENTED_POOL_H
#define BB_SEGMENTED_POOL_H
#include "engine/common/bbFlag.h"


typedef struct bbSegmentedPool bbSegmentedPool;

/**
 * @brief Creates an empty segmented memory pool.
 *
 * Segments are allocated lazily.
 *
 * @param[out] out_pool Receives the allocated pool.
 * @param element_size Size of each user element in bytes.
 * @param alignment Required alignment of each element.
 * @param elements_per_segment Number of elements per segment.
 *
 * @return bbSuccess on success, bbFail on failure.
 */
bbFlag bbSegmentedPool_new(
    bbSegmentedPool** out_pool,
    size_t element_size,
    size_t alignment,
    U32 elements_per_segment,
    U16 system);

bbFlag bbSegmentedPool_alloc(
    bbSegmentedPool* pool,
    bbHandle* out_handle,
    void** out_data
);

#endif //BB_SEGMENTED_POOL_H