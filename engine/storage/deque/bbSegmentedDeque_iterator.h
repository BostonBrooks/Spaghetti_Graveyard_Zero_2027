
#ifndef BB_SEGMENTED_DEQUE_ITERATOR_H
#define BB_SEGMENTED_DEQUE_ITERATOR_H

#include "engine/storage/deque/bbSegmentedDeque.h"
#include "engine/common/bbFlag.h"

typedef struct {
    const bbDequeSegment* segment; //Current segment
    U32 index; //index within the current segment
    size_t position; //logical index within entire deque
    size_t count; //total number of elements in the deque
} bbSegmentedDeque_iterator;

bbFlag bbSegmentedDeque_iteratorFront(
    const bbSegmentedDeque* deque,
    bbSegmentedDeque_iterator* iterator,
    const void** data);

bbFlag bbSegmentedDeque_iteratorBack(
    const bbSegmentedDeque* deque,
    bbSegmentedDeque_iterator* iterator,
    const void** data);

bbFlag bbSegmentedDeque_iteratorNext(
    bbSegmentedDeque_iterator* iterator,
    const void** data);

bbFlag bbSegmentedDeque_iteratorPrev(
    bbSegmentedDeque_iterator* iterator,
    const void** data);

bbFlag bbSegmentedDeque_iteratorGet(
    const bbSegmentedDeque_iterator* iterator,
    const void** data);

#endif //BB_SEGMENTED_DEQUE_ITERATOR_H