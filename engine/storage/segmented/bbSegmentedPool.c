#include <stddef.h>
#include <stdalign.h>

#include "engine/common/bbIntTypes.h"
#include "engine/common/bbHandle.h"
#include "engine/storage/segmented/bbSegmentedPool.h"

#include <assert.h>

#include "engine/common/bbFlag.h"
#include <stdbool.h>
#include <stdlib.h>

#define BB_POOL_IN_USE UINT32_MAX
#define BB_POOL_END_OF_LIST (UINT32_MAX-1)

typedef struct
{
    bbHandle self;
    U32 next_free;
    U8 data[];
} bbSegmentedPool_header;


typedef struct {
    U32 first_free;
    U32 free_count;

    U32 next_available;
    U32 prev_available;

    U8 data[];
} bbSegmentedPool_segment;


struct bbSegmentedPool{
    bbSegmentedPool_segment** segments;
    size_t element_size;
    size_t element_stride;
    U32 alignment;
    U32 first_available_segment;
    U32 elements_per_segment;
    U32 segment_count;
    U32 segment_capacity;
    U16 system;
};


/**
 * @brief Calculates the aligned offset of an element's user data.
 *
 * @param pool Pointer to the segmented pool.
 * @return Offset in bytes from the header to the user data.
 */
static inline size_t
bbSegmentedPool_headerOffset(const bbSegmentedPool* pool)
{
    const size_t alignment = pool->alignment;

    return (offsetof(bbSegmentedPool_header, data)
            + alignment - 1) / alignment * alignment;
}

/**
 * @brief Calculates the aligned offset of the first element in a segment.
 *
 * @param pool Pointer to the segmented pool.
 * @return Offset in bytes from the segment to its first element.
 */
static inline size_t
bbSegmentedPool_segmentOffset(const bbSegmentedPool* pool)
{
    const size_t alignment = pool->alignment;

    return (offsetof(bbSegmentedPool_segment, data)
            + alignment - 1) / alignment * alignment;
}

static inline bbSegmentedPool_header*
bbSegmentedPool_getSegmentHeader(
    const bbSegmentedPool* pool,
    bbSegmentedPool_segment* segment,
    U32 local_index) {
    assert(local_index < pool->elements_per_segment);
    size_t segment_offset = bbSegmentedPool_segmentOffset(pool);
    size_t element_stride = pool->element_stride;

    U8* base = (U8*)segment;
    bbSegmentedPool_header* header =
        (bbSegmentedPool_header*)
        (base + segment_offset + local_index * element_stride);

    return header;
}

bbFlag bbSegmentedPool_new(
    bbSegmentedPool** out_pool,
    size_t element_size,
    size_t alignment,
    U32 elements_per_segment,
    U16 system)
{
    if (out_pool == NULL)
        return bbFail;

    *out_pool = NULL;

    if (element_size == 0 ||
        elements_per_segment == 0 ||
        alignment == 0)
        return bbFail;

    // Require power-of-two alignment.
    if ((alignment & (alignment - 1)) != 0)
        return bbFail;

    // Ensure alignment supports both metadata structures.
    if (alignment < _Alignof(bbSegmentedPool_header))
        alignment = _Alignof(bbSegmentedPool_header);

    if (alignment < _Alignof(bbSegmentedPool_segment))
        alignment = _Alignof(bbSegmentedPool_segment);

    bbSegmentedPool* pool = calloc(1, sizeof(*pool));

    if (pool == NULL)
        return bbFail;

    pool->system = system;
    pool->element_size = element_size;
    pool->alignment = alignment;
    pool->elements_per_segment = elements_per_segment;

    const size_t header_offset =
        bbSegmentedPool_headerOffset(pool);

    // Check arithmetic before rounding the stride.
    if (element_size > SIZE_MAX - header_offset ||
        element_size + header_offset > SIZE_MAX - (alignment - 1))
    {
        free(pool);
        return bbFail;
    }

    const size_t unaligned_stride =
        header_offset + element_size;

    pool->element_stride =
        (unaligned_stride + alignment - 1)
        & ~(alignment - 1);

    // Check that a segment's element storage can fit in size_t.
    const size_t segment_offset =
        bbSegmentedPool_segmentOffset(pool);

    if (pool->element_stride >
        (SIZE_MAX - segment_offset) / elements_per_segment)
    {
        free(pool);
        return bbFail;
    }

    pool->segments = NULL;
    pool->first_available_segment = BB_POOL_END_OF_LIST;
    pool->segment_count = 0;
    pool->segment_capacity = 0;

    *out_pool = pool;

    return bbSuccess;
}

bbFlag bbSegmentedPool_newSegment(bbSegmentedPool* pool) {

    size_t element_stride = pool->element_stride;
    U32 elements_per_segment = pool->elements_per_segment;
    size_t segment_offset = bbSegmentedPool_segmentOffset(pool);
    size_t segment_size = segment_offset + element_stride * elements_per_segment;

    bbSegmentedPool_segment* segment =
        aligned_alloc(pool->alignment,segment_size);
    if (segment == NULL) return bbFail;

    segment->free_count = elements_per_segment;
    segment->first_free = 0;

    bbSegmentedPool_header* header;
    for (U32 i = 0; i < elements_per_segment; i++) {
        header = bbSegmentedPool_getSegmentHeader(pool,segment,i);
        header->next_free = i+1;
        //header->self.system.index = segment index * elements per segment + i
        header->self.system.system = pool->system;
        header->self.system.generation = 0;
    }

    header->next_free = BB_POOL_END_OF_LIST;

    //add segment to pool

}