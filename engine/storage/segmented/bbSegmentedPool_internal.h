#define BB_POOL_IN_USE UINT32_MAX
#define BB_POOL_END_OF_LIST (UINT32_MAX-1)
#define BB_POOL_NULL_GENERATION 0
#define BB_POOL_INITIAL_GENERATION 1
#include "engine/common/bbFlag.h"
#include "engine/common/bbHandle.h"
#include "engine/storage/segmented/bbSegmentedPool.h"

typedef bbFlag bbSegmentedPool_initFn(
    struct bbSegmentedPool* pool,
    void* object,
    void* cl);



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