#include <stddef.h>
#include <stdalign.h>
#include <stdio.h>
#include "engine/common/bbIntTypes.h"
#include "engine/common/bbHandle.h"
#include "engine/storage/segmented/bbSegmentedPool.h"

#include <assert.h>

#include "engine/common/bbFlag.h"
#include <stdbool.h>
#include <stdlib.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"


/**
 * @brief Calculates the aligned offset of an element's user data.
 *
 * @param pool Pointer to the segmented pool.
 * @return Offset in bytes from the header to the user data.
 */
static inline size_t
bbSegmentedPool_headerOffset(size_t alignment)
{
    size_t offset = offsetof(bbSegmentedPool_header, data);
    return (offset + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief Calculates the aligned offset of the first element in a segment.
 *
 * @param pool Pointer to the segmented pool.
 * @return Offset in bytes from the segment to its first element.
 */
static inline size_t
bbSegmentedPool_segmentOffset(size_t alignment)
{
    size_t offset = offsetof(bbSegmentedPool_segment, data);
    return (offset + alignment - 1) & ~(alignment - 1);
}


/**
 * @brief Returns the header associated with an object in a pool
 *
 */
static inline bbSegmentedPool_header*
    bbSegmentedPool_getObjectHeader(
        const bbSegmentedPool* pool,
        const void* object) {
    size_t offset = bbSegmentedPool_headerOffset(pool->alignment);

    bbSegmentedPool_header* header = (bbSegmentedPool_header*)((const U8*)object - offset);

    bbAssert(
        header->self.system.system == pool->system,
        "Object belongs to a different system\n"
    );


}


static inline bbSegmentedPool_header*
bbSegmentedPool_getHeader(
    const bbSegmentedPool* pool,
    bbSegmentedPool_segment* segment,
    U32 local_index) {
    assert(local_index < pool->elements_per_segment);
    size_t alignment = pool->alignment;

    size_t segment_offset = bbSegmentedPool_segmentOffset(alignment);
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
    bbWarning(elements_per_segment < BB_POOL_END_OF_LIST, "elements_per_segment too large\n");
    if (elements_per_segment >= BB_POOL_END_OF_LIST) return bbFail;

    bbWarning(elements_per_segment > 0, "Zero elements per segment\n");
    if (elements_per_segment == 0) return bbFail;

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


    size_t max_offset = offsetof(bbSegmentedPool_header, data);
    size_t segment_offset_test = offsetof(bbSegmentedPool_segment, data);

    if (segment_offset_test > max_offset)
        max_offset = segment_offset_test;

    if (alignment - 1 > SIZE_MAX - max_offset) {
        bbWarning(0==1,"Offset calculation will overflow\n");
        return bbFail;
    }


    const size_t header_offset =
        bbSegmentedPool_headerOffset(alignment);


    // Check arithmetic before rounding the stride.
    if (element_size > SIZE_MAX - header_offset ||
        element_size + header_offset > SIZE_MAX - (alignment - 1))
    {
        return bbFail;
    }

    const size_t unaligned_stride =
        header_offset + element_size;

    size_t element_stride =
    (unaligned_stride + alignment - 1)
    & ~(alignment - 1);

    if (element_stride >
    (SIZE_MAX - segment_offset_test) / elements_per_segment)
    {
        return bbFail;
    }

    const size_t segment_offset =
        bbSegmentedPool_segmentOffset(alignment);

    size_t segment_size =
    segment_offset + element_stride * elements_per_segment;
    bbAssert(segment_size % alignment == 0,
         "Segment size is not aligned\n");

    bbSegmentedPool* pool = calloc(1, sizeof(*pool));

    if (pool == NULL)
        return bbFail;

    pool->system = system;
    pool->element_size = element_size;
    pool->alignment = alignment;
    pool->elements_per_segment = elements_per_segment;
    pool->element_stride = element_stride;
    pool->segment_size =segment_size;
    pool->segments = NULL;
    pool->first_available_segment = BB_POOL_END_OF_LIST;
    pool->segment_count = 0;
    pool->segment_capacity = 0;

    *out_pool = pool;

    return bbSuccess;
}
#define BB_MIN_SEGMENT_CAPACITY 8

/**
 * @brief Allocates and initializes a new segment.
 * @param pool Pointer to an initialized segmented pool
 * @return bbSuccess is the segmented is created,
 *         bbFull if no more space can be allocated
 *         bbFail if allocation or capacity growth failed
 */
bbFlag bbSegmentedPool_newSegment(bbSegmentedPool* pool) {
    //Assert pool is in a valid state

    bbAssert(pool != NULL, "Null pool pointer\n");
    bbAssert(pool->segment_count <= pool->segment_capacity,
             "Segment count exceeds capacity\n");

    //check if the pool has a slot for a new segment, otherwise expand pool
    U32 segment_count = pool->segment_count;
    U32 segment_capacity = pool->segment_capacity;
    U32 elements_per_segment = pool->elements_per_segment;

    U32 max_capacity = UINT32_MAX  / elements_per_segment;

    if (max_capacity > BB_POOL_END_OF_LIST)
        max_capacity = BB_POOL_END_OF_LIST;

    if (segment_capacity == 0) {
        //allocate segment array

        bbAssert(segment_count == 0,
                 "Nonzero segment count with zero capacity\n");

        U32 initial_capacity =
        max_capacity < BB_MIN_SEGMENT_CAPACITY
            ? max_capacity
            : BB_MIN_SEGMENT_CAPACITY;


        bbSegmentedPool_segment** new_segments;
        new_segments = calloc(
            initial_capacity,
            sizeof(*new_segments)
            );
        bbWarning(new_segments!=NULL, "Segmented pool allocation failed\n");
        if (new_segments == NULL) return bbFail;

        pool->segments = new_segments;
        pool->segment_capacity = segment_capacity = initial_capacity;

    } else if (segment_count == segment_capacity) {
        //re-allocate segment array
        if (segment_capacity >= max_capacity) {
            bbWarning(false, "Pool at maximum segment capacity\n");
            return bbFull;
        }
        bbSegmentedPool_segment** new_segments;


        U32 new_segment_capacity;

        if (segment_capacity > max_capacity / 2) {
            new_segment_capacity = max_capacity;
        } else {
            new_segment_capacity = segment_capacity * 2;
        }

        if (new_segment_capacity > SIZE_MAX / sizeof(*new_segments)) {
            bbWarning(false, "Segment pointer array size overflow\n");
            return bbFull;
        }

        new_segments = realloc(
            pool->segments,
            sizeof(*new_segments)*new_segment_capacity
            );
        bbWarning(new_segments!=NULL, "Segmented pool reallocation failed\n");
        if (new_segments == NULL) return bbFail;

        pool->segments = new_segments;
        pool->segment_capacity = new_segment_capacity;


    }

    size_t segment_size = pool->segment_size;

    bbSegmentedPool_segment* segment =
        aligned_alloc(pool->alignment,segment_size);
    bbWarning(segment!=NULL, "Segmented pool allocation failed\n");
    if (segment == NULL) return bbFail;


    U32 current_segment = segment_count;


    segment->free_count = elements_per_segment;
    segment->first_free = 0;

    bbSegmentedPool_header* header;
    for (U32 i = 0; i < elements_per_segment; i++) {
        header = bbSegmentedPool_getHeader(pool,segment,i);
        header->next_free = i+1;
        header->self.system.index = current_segment * elements_per_segment + i;
        header->self.system.system = pool->system;
        header->self.system.generation = BB_POOL_INITIAL_GENERATION;
    }

    header->next_free = BB_POOL_END_OF_LIST;


    //add segment to pool
    pool->segments[current_segment] = segment;
    pool->segment_count = ++segment_count;

    //add segment to list
    U32 old_head = pool->first_available_segment;

    segment->prev_available = BB_POOL_END_OF_LIST;
    segment->next_available = old_head;

    if (old_head != BB_POOL_END_OF_LIST) {
        pool->segments[old_head]->prev_available = current_segment;
    }

    pool->first_available_segment = current_segment;

    return bbSuccess;


}

bbFlag bbSegmentedPool_alloc(
    bbSegmentedPool* pool,
    bbHandle* out_handle,
    void** out_data
) {

    bbAssert(pool != NULL, "Trying to alloc from a null pool\n");
    bbAssert(out_handle != NULL || out_data != NULL,
        "Allocation requires an output parameter\n");

    U32 segment_index = pool->first_available_segment;

    if (segment_index == BB_POOL_END_OF_LIST) {
        bbFlag result = bbSegmentedPool_newSegment(pool);
        if (result != bbSuccess) {

            if (out_handle != NULL) {
                *out_handle = (bbHandle){0};
            }

            if (out_data != NULL) {
                *out_data = NULL;
            }
            return result;
        }
        segment_index = pool->first_available_segment;
    }

    bbSegmentedPool_segment* segment = pool->segments[segment_index];
    bbAssert(segment->prev_available == BB_POOL_END_OF_LIST,
        "Available segment head has a predeccessor\n");
    bbAssert(segment->free_count > 0, "Segment full in available segment list\n");

    U32 first_free = segment->first_free;
    bbAssert(first_free < pool->elements_per_segment,
        "Invalid free-list head\n");
    bbSegmentedPool_header* header = bbSegmentedPool_getHeader(pool,segment,first_free);

    U32 next_free = header->next_free;

    bbAssert(next_free == BB_POOL_END_OF_LIST ||
        next_free < pool->elements_per_segment,
        "Invalid next_free index\n");
    segment->first_free = next_free;
    header->next_free = BB_POOL_IN_USE;
    segment->free_count--;

    if (segment->free_count == 0) {
        bbAssert(segment->first_free == BB_POOL_END_OF_LIST,
            "Full segment has nonempty free list\n");
        U32 next = segment->next_available;

        pool->first_available_segment = next;


        if (next != BB_POOL_END_OF_LIST) {
            pool->segments[next]->prev_available = BB_POOL_END_OF_LIST;
        }

        segment->prev_available = BB_POOL_END_OF_LIST;
        segment->next_available = BB_POOL_END_OF_LIST;
    }

    size_t header_offset = bbSegmentedPool_headerOffset(pool->alignment);

    U8* data = ((U8*)header) + header_offset;

    if (out_handle != NULL) *out_handle = header->self;
    if (out_data != NULL) *out_data = data;

    return bbSuccess;
}

bbFlag bbSegmentedPool_free(
    bbSegmentedPool* pool,
    bbHandle handle
) {
    bbAssert(pool != NULL, "Null pool pointer\n");

    // if (handle.system.generation != BB_POOL_NULL_GENERATION) {
    //     return bbNull;
    // }

    if (handle.system.system != pool->system) {
        return bbWrongSystem;
    }

    U32 index = handle.system.index;
    U32 elements_per_segment = pool->elements_per_segment;
    U32 segment_index = index / elements_per_segment;
    U32 local_index = index % elements_per_segment;

    if (segment_index >= pool->segment_count) {
        bbWarning(0==1, "Segment index out of bounds\n");
        return bbOutOfBounds;
    }


    bbSegmentedPool_segment* segment = pool->segments[segment_index];
    bbSegmentedPool_header* header = bbSegmentedPool_getHeader(pool,segment,local_index);


    if (header->next_free != BB_POOL_IN_USE) {
        bbWarning(0==1, "Double free in pool\n");
        return bbDoubleFree;
    }

    if (header->self.system.generation != handle.system.generation) {
        //bbWarning(0==1, "Wrong generation in pool free\n");
        //returning bbWrongGeneration may be intended behaviour
        return bbWrongGeneration;
    }


    header->self.system.generation++;
    if (header->self.system.generation == BB_POOL_NULL_GENERATION) {
        header->self.system.generation++;
    }

    bool was_full = (segment->free_count == 0);

    header->next_free = segment->first_free;
    segment->first_free = local_index;
    segment->free_count++;

    if (was_full) {

        bbAssert(
            segment->next_available == BB_POOL_END_OF_LIST &&
            segment->prev_available == BB_POOL_END_OF_LIST,
            "Full segment unexpectedly linked\n");
        //add segment to list
        U32 old_head = pool->first_available_segment;

        segment->prev_available = BB_POOL_END_OF_LIST;
        segment->next_available = old_head;

        if (old_head != BB_POOL_END_OF_LIST) {
            pool->segments[old_head]->prev_available = segment_index;
        }

        pool->first_available_segment = segment_index;
    }

    return bbSuccess;
}


bbFlag bbSegmentedPool_freeObject(
    bbSegmentedPool* pool,
    void* object
) {
    bbAssert(pool != NULL, "Null pool pointer\n");
    if (object == NULL) return bbNull;
    bbSegmentedPool_header* header = bbSegmentedPool_getObjectHeader(pool,object);
    return bbSegmentedPool_free(pool,header->self);
}


bbFlag bbSegmentedPool_delete(bbSegmentedPool** pool) {

    bbAssert(pool != NULL && *pool != NULL, "Trying to delete null pool\n");

    for (U32 i = 0; i < (*pool)->segment_count; i++) {
        free((*pool)->segments[i]);
    }
    free((*pool)->segments);
    free(*pool);
    *pool = NULL;
    return bbSuccess;
}
