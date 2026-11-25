
#ifndef BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H
#define BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H

#include "engine/storage/deque/bbSegmentedDeque.h"
#include "engine/common/bbFlag.h"



#define CONCAT1(x,y) x##y
#define CONCAT2(x,y) CONCAT1(x,y)


#define DECLARE_DEQUE_HEADER(BB_NAME,BB_CLASS, BB_SEGMENT_SIZE)\
\
typedef struct {\
    const CONCAT2(BB_NAME,_dequeSegment)* segment; /*Current segment*/\
    U32 index; /*index within the current segment*/\
    size_t position; /*logical index within entire deque*/\
    size_t count; /*total number of elements in the deque*/\
} CONCAT2(BB_NAME,_iterator);\

bbFlag CONCAT2(BB_NAME,_iteratorFront)(
    const bbSegmentedDeque* deque,
    CONCAT2(BB_NAME,_iterator)* iterator,
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

#endif //BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H