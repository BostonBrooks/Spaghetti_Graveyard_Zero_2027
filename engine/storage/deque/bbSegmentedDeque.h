#ifndef BB_SEGMENTED_DEQUE_H
#define BB_SEGMENTED_DEQUE_H
#include <stdalign.h>
#include <stddef.h>

#include "engine/common/bbFlag.h"
#include "engine/common/bbIntTypes.h"

#define BBSEGMENTEDDEQUE_SIZEOF 128
#define BBSEGMENTEDDEQUE_SEGMENT_SIZE 12

typedef struct
{
    U8 data[BBSEGMENTEDDEQUE_SIZEOF];
} bbDummyStruct;


typedef struct bbDequeSegment {
    struct bbDequeSegment* prev;
    struct bbDequeSegment* next;

    alignas(8) bbDummyStruct dummy[BBSEGMENTEDDEQUE_SEGMENT_SIZE];
} bbDequeSegment;

typedef struct bbSegmentedDeque {
    bbDequeSegment* head;
    bbDequeSegment* tail;
    bbDequeSegment* spare_segment;

    size_t count;
    U32 begin;
    U32 end;
} bbSegmentedDeque;

bbFlag bbSegmentedDeque_init(bbSegmentedDeque* deque);
bbFlag bbSegmentedDeque_destroy(bbSegmentedDeque* deque);

bbFlag bbSegmentedDeque_pushFront(
bbSegmentedDeque* deque, bbDummyStruct* element);

bbFlag bbSegmentedDeque_pushBack(
bbSegmentedDeque* deque, bbDummyStruct* element);

bbFlag bbSegmentedDeque_popFront(
bbSegmentedDeque* deque, bbDummyStruct* element);

bbFlag bbSegmentedDeque_popBack(
bbSegmentedDeque* deque, bbDummyStruct* element);

bbFlag bbSegmentedDeque_peekFront(
const bbSegmentedDeque* deque, bbDummyStruct* element);

bbFlag bbSegmentedDeque_peekBack(
const bbSegmentedDeque* deque, bbDummyStruct* element);

#endif //BB_BB_SEGMENTED_DEQUE_H