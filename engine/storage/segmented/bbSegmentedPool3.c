#include <stddef.h>
#include <stdlib.h>

#include "engine/common/bbIntTypes.h"
#include "engine/common/bbTerminal.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbHandle.h"
#include "engine/storage/segmented/bbSegmentedPool.h"
#define BB_POOL_IN_USE UINT32_MAX
#define BB_POOL_END_OF_LIST (UINT32_MAX-1)
#define BB_POOL_NULL_GENERATION 0
#define BB_POOL_INITIAL_GENERATION 1



typedef bbFlag bbSegmentedPool_initFn(
    bbSegmentedPool* pool,
    void* object,
    void* cl);


//metadata stored at the end of each slot
typedef struct {
    bbHandle self;
    U32 next_free;
} bbSegmentedPool_metaData;

//Header for storing metadata at the beginning of each segment
typedef struct {
    U32 first_free; //first free slot
    U32 free_count; //number of free slots

    U32 next_available; //next available segment
    U32 prev_available; //prev available segment
} bbSegmentedPool_segment;

typedef struct bbSegmentedPool{
    bbSegmentedPool_segment** segments;
    size_t alignment;
    size_t element_stride;
    size_t data_offset;
    size_t segment_size;
    size_t metadata_offset;
    U32 segment_shift;
    U32 segment_mask;
    U32 first_available_segment;
    U32 elements_per_segment;
    U32 segment_count;
    U32 segment_capacity;
    U16 system;
    bbSegmentedPool_initFn* init_fn;
    void* init_cl;
} bbSegmentedPool;



//object must be an element stored in the pool.
static inline bbSegmentedPool_metaData*
    bbSegmentedPool_getObjectMetaData(
        const bbSegmentedPool* pool,
        void* object) {

    bbSegmentedPool_metaData* metadata =
        (bbSegmentedPool_metaData*)((U8*)object
            + pool->element_stride
            - sizeof(bbSegmentedPool_metaData));
    bbAssert(
        metadata->self.system.system == pool->system,
        "Object belongs to a different system\n"
    );

    return metadata;

}

static inline bbSegmentedPool_metaData*
    bbSegmentedPool_getMetaData(
    const bbSegmentedPool* pool,
    bbSegmentedPool_segment* segment,
    U32 local_index) {

    bbAssert(local_index<pool->elements_per_segment,
        "local index out of bounds\n");

    void* object = ((U8*)segment)
        + pool->data_offset
        + pool->element_stride * local_index;

    bbSegmentedPool_metaData* metadata =
        (bbSegmentedPool_metaData*)((U8*)object
            + pool->metadata_offset);

    return metadata;

}



bbFlag bbSegmentedPool_new(
    bbSegmentedPool** out_pool,
    size_t element_size,
    size_t alignment,
    U32 elements_per_segment,
    U16 system,
    bbSegmentedPool_initFn* init_fn,
    void* init_cl) {


    if (out_pool == NULL) {
        bbWarning(out_pool != NULL, "NULL pointer provided");
        return bbFail;
    }

    *out_pool = NULL;

    bbWarning(elements_per_segment < BB_POOL_END_OF_LIST, "elements_per_segment too large\n");
    if (elements_per_segment >= BB_POOL_END_OF_LIST) return bbFail;

    bbWarning(element_size > 0, "Zero element size\n");
    if (element_size == 0) return bbFail;


    bbWarning(elements_per_segment > 0, "Zero elements per segment\n");
    if (elements_per_segment == 0) return bbFail;


    bbWarning(alignment > 0, "Zero alignment\n");
    if (alignment == 0) return bbFail;

    if ((alignment & (alignment - 1)) != 0) {
        bbWarning(0==1, "Alignment must be a power of 2\n");
        return bbFail;
    }

    if (alignment < _Alignof(bbSegmentedPool_metaData))
        alignment = _Alignof(bbSegmentedPool_metaData);

    if (alignment < _Alignof(bbSegmentedPool_segment))
        alignment = _Alignof(bbSegmentedPool_segment);

    size_t metadata_size = sizeof(bbSegmentedPool_metaData);

    if (element_size > SIZE_MAX - metadata_size) {
        bbWarning(0==1, "Slot size overflow\n");
        return bbFail;
    }

    size_t slot_size = element_size + metadata_size;

    if (slot_size > SIZE_MAX - (alignment - 1)) {
        bbWarning(0==1, "Slot alignment overflow\n");
        return bbFail;
    }

    size_t element_stride =
        (slot_size + alignment - 1) & ~(alignment - 1);

    if (sizeof(bbSegmentedPool_segment) > SIZE_MAX - (alignment - 1)) {
        bbWarning(0==1, "segment alignment overflow\n");
        return bbFail;
    }

    size_t data_offset =
        (sizeof(bbSegmentedPool_segment) + (alignment - 1)) & ~(alignment - 1);

    if (elements_per_segment >
        (SIZE_MAX - data_offset) / element_stride) {
        bbWarning(0==1, "element size overflow\n");
        return bbFail;
    }

    size_t segment_size =
        data_offset + element_stride * elements_per_segment;


    bbSegmentedPool* pool = calloc(1, sizeof(*pool));

    if (pool == NULL)
        return bbFail;


    U32 segment_shift = 0;
    U32 n = elements_per_segment;

    //bbDebug("elements per segment = %u\n", elements_per_segment);

    while (n > 1) {
        n >>= 1;
        segment_shift++;
    }

    pool->segment_shift = segment_shift;

    pool->system = system;
    pool->alignment = alignment;
    pool->element_stride = element_stride;
    pool->data_offset = data_offset;
    pool->segment_size = segment_size;
    pool->segments = NULL;
    pool->first_available_segment = BB_POOL_END_OF_LIST;
    pool->elements_per_segment = elements_per_segment;
    pool->segment_count = 0;
    pool->segment_capacity = 0;
    pool->init_fn = init_fn;
    pool->init_cl = init_cl;

    pool->segment_mask = elements_per_segment - 1;
    pool->metadata_offset =
        element_stride - sizeof(bbSegmentedPool_metaData);
    *out_pool = pool;
    return bbSuccess;
}


#define BB_MIN_SEGMENT_CAPACITY 8


static bbFlag bbSegmentedPool_newSegment(bbSegmentedPool* pool) {
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

    size_t max_segment_array = SIZE_MAX / sizeof (bbSegmentedPool_segment*);

    if (max_capacity > max_segment_array)
        max_capacity = (U32)max_segment_array;

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

    bbSegmentedPool_metaData* meta_data;
    for (U32 i = 0; i < elements_per_segment; i++) {
        meta_data = bbSegmentedPool_getMetaData(pool,segment,i);
        meta_data->next_free = i+1;
        meta_data->self.system.index = current_segment * elements_per_segment + i;
        meta_data->self.system.system = pool->system;
        meta_data->self.system.generation = BB_POOL_INITIAL_GENERATION;
    }
    meta_data->next_free = BB_POOL_END_OF_LIST;


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


    if (out_handle != NULL) *out_handle = (bbHandle){0};
    if (out_data != NULL) *out_data = NULL;

    U32 segment_index = pool->first_available_segment;

    if (segment_index == BB_POOL_END_OF_LIST) {
        bbFlag result = bbSegmentedPool_newSegment(pool);
        if (result != bbSuccess) {
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
    bbSegmentedPool_metaData* meta_data = bbSegmentedPool_getMetaData(pool,segment,first_free);

    U32 next_free = meta_data->next_free;

    bbAssert(next_free == BB_POOL_END_OF_LIST ||
        next_free < pool->elements_per_segment,
        "Invalid next_free index\n");
    segment->first_free = next_free;
    meta_data->next_free = BB_POOL_IN_USE;
    segment->free_count--;


    bbAssert(
        (segment->free_count == 0) ==
        (segment->first_free == BB_POOL_END_OF_LIST),
        "Inconsistent segment free list\n");

    if (segment->free_count == 0) {
        U32 next = segment->next_available;

        pool->first_available_segment = next;


        if (next != BB_POOL_END_OF_LIST) {
            pool->segments[next]->prev_available = BB_POOL_END_OF_LIST;
        }

        segment->prev_available = BB_POOL_END_OF_LIST;
        segment->next_available = BB_POOL_END_OF_LIST;
    }


    U8* data  = ((U8*)segment)
        + pool->data_offset
        + pool->element_stride * first_free;

    if (pool->init_fn != NULL) {
        bbFlag flag = pool->init_fn(pool, data, pool->init_cl);

        if (flag != bbSuccess) {
            bbFlag rollback = bbSegmentedPool_free(pool, meta_data->self);
            bbAssert(rollback == bbSuccess,
                "Failed to roll back object initialisation\n");

            return flag;
        }
    }

    if (out_handle != NULL) *out_handle = meta_data->self;
    if (out_data != NULL) *out_data = data;

    return bbSuccess;
}

bbFlag bbSegmentedPool_lookup(bbSegmentedPool* pool,
    bbHandle handle,
    void** out_data) {
    bbAssert(pool != NULL, "Null pool pointer\n");
    bbAssert(out_data != NULL, "Null output pointer\n");

    *out_data = NULL;

    if (handle.system.generation == BB_POOL_NULL_GENERATION) {
        return bbNull;
    }

    if (handle.system.system != pool->system) {
        bbWarning(0==1, "Trying to lookup handle from wrong system\n");
        return bbWrongSystem;
    }


    U32 segment_index = handle.system.index >> pool->segment_shift;
    U32 local_index = handle.system.index & pool->segment_mask;

    if (segment_index >= pool->segment_count) {
        bbWarning(0==1, "Segment index out of bounds\n");
        return bbOutOfBounds;
    }

    bbSegmentedPool_segment* segment = pool->segments[segment_index];

    U8* data  = ((U8*)segment)
    + pool->data_offset
    + pool->element_stride * local_index;

    bbSegmentedPool_metaData* meta_data =
    (bbSegmentedPool_metaData*)(data
        + pool->metadata_offset);


    bbAssert(meta_data->self.system.system == pool->system,
        "Corrupt metadata system ID\n");

    if (handle.system.generation != meta_data->self.system.generation) {
        bbWarning(0==1, "Trying to lookup a stale handle\n");
        return bbWrongGeneration;
    }

    if (meta_data->next_free != BB_POOL_IN_USE) {
        bbWarning(0==1, "Trying to look up an element not allocated\n");
        return bbNotAllocated;
    }




    *out_data = data;

    return bbSuccess;
}

bbFlag bbSegmentedPool_reverseLookup(
    bbSegmentedPool* pool,
    void* object,
    bbHandle* out_handle
) {
    bbAssert(pool != NULL, "Null pool pointer\n");
    bbAssert(out_handle != NULL, "Null handle pointer\n");

    *out_handle = (bbHandle){0};

    if (object == NULL) return bbNull;

    bbSegmentedPool_metaData* metadata =
        (bbSegmentedPool_metaData*)((U8*)object
        + pool->element_stride
        - sizeof(bbSegmentedPool_metaData));

    if (metadata->self.system.system != pool->system) {
        return bbWrongSystem;
    }

    if (metadata->next_free != BB_POOL_IN_USE) {
        return bbNotAllocated;
    }

    *out_handle = metadata->self;
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
    bbSegmentedPool_metaData* meta_data = bbSegmentedPool_getMetaData(pool,segment,local_index);


    if (meta_data->next_free != BB_POOL_IN_USE) {
        bbWarning(0==1, "Double free in pool\n");
        return bbDoubleFree;
    }

    if (meta_data->self.system.generation != handle.system.generation) {
        //bbWarning(0==1, "Wrong generation in pool free\n");
        //returning bbWrongGeneration may be intended behaviour
        return bbWrongGeneration;
    }


    meta_data->self.system.generation++;
    if (meta_data->self.system.generation == BB_POOL_NULL_GENERATION) {
        meta_data->self.system.generation++;
    }

    bool was_full = (segment->free_count == 0);

    meta_data->next_free = segment->first_free;
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
    bbSegmentedPool_metaData* meta_data = bbSegmentedPool_getObjectMetaData(pool,object);
    return bbSegmentedPool_free(pool,meta_data->self);
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