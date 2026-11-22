/** @file bbSegmentedPool_internal.h
* @brief Internal definitions for the segmented pool allocator.
*
* This file contains the definitions used in bbSegmentedPool.c
* These should be left as opaque types except for debugging.
* @warning Do not include this header in ordinary engine code.
*
*/


#ifndef BB_SEGMENTED_POOL_INTERNAL_H
#define BB_SEGMENTED_POOL_INTERNAL_H

#include "engine/storage/segmented/bbSegmentedPool.h"

#define BB_POOL_IN_USE UINT32_MAX
#define BB_POOL_END_OF_LIST (UINT32_MAX-1)
#define BB_POOL_NULL_GENERATION 0
#define BB_POOL_INITIAL_GENERATION 1

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
    size_t segment_size;
    size_t alignment;
    U32 first_available_segment;
    U32 elements_per_segment;
    U32 segment_count;
    U32 segment_capacity;
    U16 system;
    bbSegmentedPool_initFn* init_fn;
    void* init_cl;
};


#endif //BB_SEGMENTED_POOL_INTERNAL_H