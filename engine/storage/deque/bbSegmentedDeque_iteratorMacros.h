
#ifndef BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H
#define BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H

#include <stddef.h>
#include <stdbool.h>

#include "engine/common/bbFlag.h"



#define CONCAT1(x,y) x##y
#define CONCAT2(x,y) CONCAT1(x,y)


#define DECLARE_DEQUE_ITERATOR_HEADER(BB_NAME,BB_CLASS, BB_SEGMENT_SIZE)\
\
typedef struct {\
    const CONCAT2(BB_NAME,_dequeSegment)* segment; /*Current segment*/\
    U32 index; /*index within the current segment*/\
    size_t position; /*logical index within entire deque*/\
    size_t count; /*total number of elements in the deque*/\
} CONCAT2(BB_NAME,_dequeIterator);\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorFront)(\
    const CONCAT2(BB_NAME,_deque)* deque,\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data);\
\
bbFlag  CONCAT2(BB_NAME,_dequeIteratorBack)(\
    const CONCAT2(BB_NAME,_deque)* deque,\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data);\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorNext)(\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data);\
\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorPrev)(\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data);\
\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorGet)(\
    const CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data);


#define DECLARE_DEQUE_ITERATOR_SOURCE(BB_NAME,BB_CLASS, BB_SEGMENT_SIZE)\
\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorFront)(\
    const CONCAT2(BB_NAME,_deque)* deque,\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data)\
{\
    if (deque->count == 0) {\
        if(data != NULL) *data = NULL;\
        return bbEmpty;\
    }\
    iterator->segment = deque->head;\
    iterator->index = deque->begin;\
    iterator->position = 0;\
    iterator->count = deque->count;\
\
    if (data != NULL) {\
        *data = &deque->head->data[iterator->index];\
    }\
    return bbSuccess;\
}\
bbFlag  CONCAT2(BB_NAME,_dequeIteratorBack)(\
    const CONCAT2(BB_NAME,_deque)* deque,\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data)\
\
{\
    if (deque->count == 0) {\
        if(data != NULL) *data = NULL;\
        return bbEmpty;\
    }\
    iterator->segment = deque->tail;\
    iterator->index = deque->end -1;\
    iterator->position = deque->count-1;\
    iterator->count = deque->count;\
\
    if (data != NULL) {\
        *data = &deque->tail->data[iterator->index];\
    }\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorNext)(\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data)\
{\
    if (iterator->position >= iterator->count\
        || iterator->index >= BB_SEGMENT_SIZE\
        || iterator->segment == NULL) {\
        if (data != NULL) *data = NULL;\
        bbWarning(false, "Invalid deque iterator\n");\
        return bbFail;\
    }\
\
\
    if (iterator->position == iterator->count-1) {\
        if (data != NULL) *data = NULL;\
        return bbEmpty;\
    }\
\
    if (iterator->index == BB_SEGMENT_SIZE - 1) {\
        bbAssert(iterator->segment->next != NULL, "Unexpected end of list\n");\
        iterator->segment = iterator->segment->next;\
        iterator->index = 0;\
    } else {\
        iterator->index++;\
    }\
    iterator->position++;\
\
    if (data != NULL) {\
        *data = &iterator->segment->data[iterator->index];\
    }\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorPrev)(\
    CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data)\
{\
    if (iterator->position >= iterator->count\
        || iterator->index >= BB_SEGMENT_SIZE\
        || iterator->segment == NULL) {\
        if (data != NULL) *data = NULL;\
        bbWarning(false, "Invalid deque iterator\n");\
        return bbFail;\
    }\
\
\
\
    if (iterator->position == 0) {\
        if (data != NULL) *data = NULL;\
        return bbEmpty;\
    }\
\
    if (iterator->index == 0) {\
        bbAssert(iterator->segment->prev != NULL, "Unexpected end of list\n");\
        iterator->segment = iterator->segment->prev;\
        iterator->index = BB_SEGMENT_SIZE - 1;\
    } else {\
        iterator->index--;\
    }\
    iterator->position--;\
\
    if (data != NULL) {\
        *data = &iterator->segment->data[iterator->index];\
    }\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_dequeIteratorGet)(\
    const CONCAT2(BB_NAME,_dequeIterator)* iterator,\
    const BB_CLASS** data)\
{\
\
    if (iterator->position >= iterator->count\
        || iterator->index >= BB_SEGMENT_SIZE\
        || iterator->segment == NULL) {\
        if (data != NULL) *data = NULL;\
        bbWarning(false, "Invalid deque iterator\n");\
        return bbFail;\
    }\
\
\
    U32 index = iterator->index;\
\
    if (data != NULL) {\
        *data = &iterator->segment->data[index];\
    }\
\
    return bbSuccess;\
\
}


#endif //BB_SEGMENTED_DEQUE_ITERATOR_MACROS_H