#include "engine/storage/deque/bbSegmentedDeque.h"

#include <stdlib.h>

#include "engine/common/bbTerminal.h"


bbFlag bbSegmentedDeque_init(bbSegmentedDeque* deque) {
    *deque = (bbSegmentedDeque){0};
    return bbSuccess;
}

bbFlag bbSegmentedDeque_destroy(bbSegmentedDeque* deque) {

    bbDequeSegment* segment = deque->head;

    while (segment != NULL) {
        bbDequeSegment* next_segment = segment->next;
        free(segment);
        segment = next_segment;
    }

    free(deque->spare_segment);
    *deque = (bbSegmentedDeque){0};

    return bbSuccess;
}


bbFlag bbSegmentedDeque_expandFront(bbSegmentedDeque* deque) {
    bbAssert(deque->begin == 0, "Cannot expand front, begin != 0\n");

    bbDequeSegment* segment = deque->spare_segment;
    if (segment != NULL) {
        deque->spare_segment = NULL;
    } else {
        segment = malloc(sizeof(bbDequeSegment));
        if (segment == NULL) {
            bbWarning(segment != NULL, "malloc failed");
            return bbFail;
        }
    }

    segment->prev = NULL;
    segment->next = deque->head;

    if (deque->head != NULL) {
        deque->head->prev = segment;
    } else {
        deque->tail = segment;
        deque->end = BBSEGMENTEDDEQUE_SEGMENT_SIZE;
    }
    deque->head = segment;

    deque->begin = BBSEGMENTEDDEQUE_SEGMENT_SIZE;

    return bbSuccess;

}


bbFlag bbSegmentedDeque_expandBack(bbSegmentedDeque* deque) {
    bbAssert(
        deque->tail == NULL ||
        deque->end == BBSEGMENTEDDEQUE_SEGMENT_SIZE,
        "Cannot expand back, begin != maximum\n");

    bbDequeSegment* segment = deque->spare_segment;
    if (segment != NULL) {
        deque->spare_segment = NULL;
    } else {
        segment = malloc(sizeof(bbDequeSegment));
        if (segment == NULL) {
            bbWarning(segment != NULL, "malloc failed\n");
            return bbFail;
        }
    }

    segment->next = NULL;
    segment->prev = deque->tail;

    if (deque->tail != NULL) {
        deque->tail->next = segment;
    }else {
        deque->head = segment;
        deque->begin = 0;
    }
    deque->tail = segment;

    deque->end = 0;

    return bbSuccess;
}

bbFlag bbSegmentedDeque_pushFront(
    bbSegmentedDeque* deque,
    bbDummyStruct* element)
{

    bbAssert(element != NULL, "Cannot push a null element\n");

    if (deque->begin == 0) {
        bbFlag flag = bbSegmentedDeque_expandFront(deque);
        bbWarning(flag == bbSuccess,
            "bbSegmentedDeque_expandFront() failed\n");
        if (flag != bbSuccess)  return flag;
    }

    U32 segment_index = --deque->begin;
    bbDequeSegment* segment = deque->head;
    bbDummyStruct* object = &segment->dummy[segment_index];
    *object = *element;
    deque->count++;

    return bbSuccess;

}

bbFlag bbSegmentedDeque_pushBack(
    bbSegmentedDeque* deque,
    bbDummyStruct* element)
{

    bbAssert(element != NULL, "Cannot push a null element\n");

    if (deque->end == BBSEGMENTEDDEQUE_SEGMENT_SIZE
        || deque->tail == NULL) {

        bbFlag flag = bbSegmentedDeque_expandBack(deque);
        bbWarning(flag == bbSuccess,
            "bbSegmentedDeque_expandBack() failed\n");
        if (flag != bbSuccess)  return flag;
    }

    U32 segment_index = deque->end++;
    bbDequeSegment* segment = deque->tail;
    bbDummyStruct* object = &segment->dummy[segment_index];
    *object = *element;
    deque->count++;



    return bbSuccess;

}


static inline void bbSegmentedDeque_removeSegment(
    bbSegmentedDeque* deque,
    bbDequeSegment* segment) {
    if (segment->prev != NULL) {
        segment->prev->next = segment->next;
    } else {
        deque->head = segment->next;
    }
    if (segment->next != NULL) {
        segment->next->prev = segment->prev;
    } else {
        deque->tail = segment->prev;
    }

    if (deque->spare_segment == NULL) {
        segment->prev = NULL;
        segment->next = NULL;
        deque->spare_segment = segment;
    }
    else {
        free(segment);
    }
}



bbFlag bbSegmentedDeque_popFront(
    bbSegmentedDeque* deque, bbDummyStruct* element) {
    if (deque->count == 0) {
        return bbEmpty;
    }

    U32 index = deque->begin++;
    bbDequeSegment* segment = deque->head;
    deque->count--;
    if (element != NULL) {
        bbAssert(index < BBSEGMENTEDDEQUE_SEGMENT_SIZE, "index out of bounds\n");
        *element = segment->dummy[index];
    }

    if (deque->count == 0) {
        bbSegmentedDeque_removeSegment(deque, segment);
        deque->begin = 0;
        deque->end = 0;
    } else if (deque->begin == BBSEGMENTEDDEQUE_SEGMENT_SIZE) {
        bbSegmentedDeque_removeSegment(deque, segment);
        deque->begin = 0;
    }
    return bbSuccess;
}


bbFlag bbSegmentedDeque_popBack(
    bbSegmentedDeque* deque, bbDummyStruct* element) {
    if (deque->count == 0) {
        return bbEmpty;
    }

    U32 index = --deque->end;
    bbDequeSegment* segment = deque->tail;
    deque->count--;
    bbAssert(index < BBSEGMENTEDDEQUE_SEGMENT_SIZE, "index out of bounds\n");

    if (element != NULL) {
        *element = segment->dummy[index];
    }

    if (deque->count == 0) {
        bbSegmentedDeque_removeSegment(deque, segment);
        deque->begin = 0;
        deque->end = 0;
    } else if (deque->end == 0) {
        bbSegmentedDeque_removeSegment(deque, segment);
        deque->end = BBSEGMENTEDDEQUE_SEGMENT_SIZE;
    }
    return bbSuccess;
}