#include "engine/storage/deque/bbSegmentedDeque_iterator.h"

#include <stdlib.h>

#include "engine/common/bbTerminal.h"


bbFlag bbSegmentedDeque_iteratorFront(
    const bbSegmentedDeque* deque,
    bbSegmentedDeque_iterator* iterator,
    const void** data)
{
    if (deque->count == 0) {
        *data = NULL;
        return bbEmpty;
    }
    iterator->segment = deque->head;
    iterator->index = deque->begin;
    iterator->position = 0;
    iterator->count = deque->count;

    if (data != NULL) {
        *data = &deque->head->dummy[iterator->index];
    }
    return bbSuccess;
}
bbFlag bbSegmentedDeque_iteratorBack(
    const bbSegmentedDeque* deque,
    bbSegmentedDeque_iterator* iterator,
    const void** data)

{
    if (deque->count == 0) {
        *data = NULL;
        return bbEmpty;
    }
    iterator->segment = deque->tail;
    iterator->index = deque->end -1;
    iterator->position = deque->count-1;
    iterator->count = deque->count;

    if (data != NULL) {
        *data = &deque->head->dummy[iterator->index];
    }
    return bbSuccess;
}

bbFlag bbSegmentedDeque_iteratorNext(
    bbSegmentedDeque_iterator* iterator,
    const void** data)
{
    if (iterator->position >= iterator->count
        || iterator->index >= BBSEGMENTEDDEQUE_SEGMENT_SIZE
        || iterator->segment == NULL) {
        if (data != NULL) *data = NULL;
        bbWarning(false, "Invalid deque iterator\n");
        return bbFail;
    }


    if (iterator->position == iterator->count-1) {
        if (data != NULL) *data = NULL;
        return bbEmpty;
    }

    if (iterator->index == BBSEGMENTEDDEQUE_SEGMENT_SIZE - 1) {
        bbAssert(iterator->segment->next != NULL, "Unexpected end of list\n");
        iterator->segment = iterator->segment->next;
        iterator->index = 0;
    } else {
        iterator->index++;
    }
    iterator->position++;

    if (data != NULL) {
        *data = &iterator->segment->dummy[iterator->index];
    }
    return bbSuccess;
}

bbFlag bbSegmentedDeque_iteratorPrev(
    bbSegmentedDeque_iterator* iterator,
    const void** data)
{
    if (iterator->position >= iterator->count
        || iterator->index >= BBSEGMENTEDDEQUE_SEGMENT_SIZE
        || iterator->segment == NULL) {
        if (data != NULL) *data = NULL;
        bbWarning(false, "Invalid deque iterator\n");
        return bbFail;
        }



    if (iterator->position == 0) {
        if (data != NULL) *data = NULL;
        return bbEmpty;
    }

    if (iterator->index == 0) {
        bbAssert(iterator->segment->prev != NULL, "Unexpected end of list\n");
        iterator->segment = iterator->segment->prev;
        iterator->index = BBSEGMENTEDDEQUE_SEGMENT_SIZE - 1;
    } else {
        iterator->index--;
    }
    iterator->position--;

    if (data != NULL) {
        *data = &iterator->segment->dummy[iterator->index];
    }
    return bbSuccess;
}

bbFlag bbSegmentedDeque_iteratorGet(
    const bbSegmentedDeque_iterator* iterator,
    const void** data)
{

    if (iterator->position >= iterator->count
        || iterator->index >= BBSEGMENTEDDEQUE_SEGMENT_SIZE
        || iterator->segment == NULL) {
        if (data != NULL) *data = NULL;
        bbWarning(false, "Invalid deque iterator\n");
        return bbFail;
        }


    U32 index = iterator->index;

    if (data != NULL) {
        *data = &iterator->segment->dummy[index];
    }

    return bbSuccess;

}
