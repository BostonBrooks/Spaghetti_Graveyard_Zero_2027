#ifndef BB_SEGMENTED_DEQUE_MACROS
#define BB_SEGMENTED_DEQUE_MACROS
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "engine/common/bbFlag.h"
#include "engine/common/bbIntTypes.h"
#include "engine/common/bbTerminal.h"

#define CONCAT1(x,y) x##y
#define CONCAT2(x,y) CONCAT1(x,y)

#define DECLARE_DEQUE_HEADER(BB_NAME,BB_CLASS, BB_SEGMENT_SIZE)\
    \
    _Static_assert((BB_SEGMENT_SIZE) > 0, "Zero elements per segment");\
    _Static_assert((BB_SEGMENT_SIZE) <= UINT32_MAX,\
        "Segment size exceeds index range");\
    _Static_assert(alignof(BB_CLASS) <= alignof(max_align_t),\
         "over-aligned element types are not supported");\
    \
    typedef struct CONCAT2(BB_NAME,_dequeSegment) {\
    struct CONCAT2(BB_NAME,_dequeSegment)* prev;\
    struct CONCAT2(BB_NAME,_dequeSegment)* next;\
\
    BB_CLASS data[BB_SEGMENT_SIZE];\
} CONCAT2(BB_NAME,_dequeSegment);\
\
\
typedef struct BB_NAME {\
    CONCAT2(BB_NAME,_dequeSegment)* head;\
    CONCAT2(BB_NAME,_dequeSegment)* tail;\
    CONCAT2(BB_NAME,_dequeSegment)* spare_segment;\
\
    size_t count;\
    U32 begin;\
    U32 end;\
} BB_NAME;\
\
\
bbFlag CONCAT2(BB_NAME,_init)(BB_NAME* deque);\
bbFlag CONCAT2(BB_NAME,_destroy)(BB_NAME* deque);\
\
bbFlag CONCAT2(BB_NAME,_pushFront)(\
BB_NAME* deque, BB_CLASS* element);\
\
bbFlag CONCAT2(BB_NAME,_pushBack)(\
BB_NAME* deque, BB_CLASS* element);\
\
bbFlag CONCAT2(BB_NAME,_popFront)(\
BB_NAME* deque, BB_CLASS* element);\
\
bbFlag CONCAT2(BB_NAME,_popBack)(\
BB_NAME* deque, BB_CLASS* element);\
\
bbFlag CONCAT2(BB_NAME,_peekFront)(\
const BB_NAME* deque, BB_CLASS* element);\
\
bbFlag CONCAT2(BB_NAME,_peekBack)(\
const BB_NAME* deque, BB_CLASS* element);\


#define DECLARE_DEQUE_SOURCE(BB_NAME,BB_CLASS, BB_SEGMENT_SIZE)\
bbFlag CONCAT2(BB_NAME,_init)(BB_NAME* deque) {\
    *deque = (BB_NAME){0};\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_destroy)(BB_NAME* deque) {\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->head;\
\
    while (segment != NULL) {\
        CONCAT2(BB_NAME,_dequeSegment)* next_segment = segment->next;\
        free(segment);\
        segment = next_segment;\
    }\
\
    free(deque->spare_segment);\
    *deque = (BB_NAME){0};\
\
    return bbSuccess;\
}\
\
\
static bbFlag CONCAT2(BB_NAME,_expandFront)(BB_NAME* deque) {\
    bbAssert(deque->begin == 0, "Cannot expand front, begin != 0\n");\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->spare_segment;\
    if (segment != NULL) {\
        deque->spare_segment = NULL;\
    } else {\
        segment = malloc(sizeof(CONCAT2(BB_NAME,_dequeSegment)));\
        if (segment == NULL) {\
            bbWarning(segment != NULL, "malloc failed");\
            return bbFail;\
        }\
    }\
\
    segment->prev = NULL;\
    segment->next = deque->head;\
\
    if (deque->head != NULL) {\
        deque->head->prev = segment;\
    } else {\
        deque->tail = segment;\
        deque->end = BB_SEGMENT_SIZE;\
    }\
    deque->head = segment;\
\
    deque->begin = BB_SEGMENT_SIZE;\
\
    return bbSuccess;\
\
}\
\
\
static bbFlag CONCAT2(BB_NAME,_expandBack)(BB_NAME* deque) {\
    bbAssert(\
        deque->tail == NULL ||\
        deque->end == BB_SEGMENT_SIZE,\
        "Cannot expand back, end != maximum\n");\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->spare_segment;\
    if (segment != NULL) {\
        deque->spare_segment = NULL;\
    } else {\
        segment = malloc(sizeof(CONCAT2(BB_NAME,_dequeSegment)));\
        if (segment == NULL) {\
            bbWarning(segment != NULL, "malloc failed\n");\
            return bbFail;\
        }\
    }\
\
    segment->next = NULL;\
    segment->prev = deque->tail;\
\
    if (deque->tail != NULL) {\
        deque->tail->next = segment;\
    }else {\
        deque->head = segment;\
        deque->begin = 0;\
    }\
    deque->tail = segment;\
\
    deque->end = 0;\
\
    return bbSuccess;\
}\
\
bbFlag CONCAT2(BB_NAME,_pushFront)(\
    BB_NAME* deque, BB_CLASS* element)\
{\
\
    bbAssert(element != NULL, "Cannot push a null element\n");\
\
    if (deque->begin == 0) {\
        bbFlag flag = CONCAT2(BB_NAME,_expandFront)(deque);\
        bbWarning(flag == bbSuccess,\
            "Segemnted Deque _expandFront() failed\n");\
        if (flag != bbSuccess)  return flag;\
    }\
\
    U32 segment_index = --deque->begin;\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->head;\
    BB_CLASS* object = &segment->data[segment_index];\
    *object = *element;\
    deque->count++;\
\
    return bbSuccess;\
\
}\
\
bbFlag CONCAT2(BB_NAME,_pushBack)(\
    BB_NAME* deque, BB_CLASS* element)\
{\
\
    bbAssert(element != NULL, "Cannot push a null element\n");\
\
    if (deque->end == BB_SEGMENT_SIZE\
        || deque->tail == NULL) {\
\
        bbFlag flag = CONCAT2(BB_NAME,_expandBack)(deque);\
        bbWarning(flag == bbSuccess,\
            "Segemnted Deque _expandBack() failed\n");\
        if (flag != bbSuccess)  return flag;\
    }\
\
    U32 segment_index = deque->end++;\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->tail;\
    BB_CLASS* object = &segment->data[segment_index];\
    *object = *element;\
    deque->count++;\
    \
    return bbSuccess;\
}\
\
\
static inline void CONCAT2(BB_NAME,_removeSegment)(\
    BB_NAME* deque,\
    CONCAT2(BB_NAME,_dequeSegment)* segment) {\
    if (segment->prev != NULL) {\
        segment->prev->next = segment->next;\
    } else {\
        deque->head = segment->next;\
    }\
    if (segment->next != NULL) {\
        segment->next->prev = segment->prev;\
    } else {\
        deque->tail = segment->prev;\
    }\
\
    if (deque->spare_segment == NULL) {\
        segment->prev = NULL;\
        segment->next = NULL;\
        deque->spare_segment = segment;\
    }\
    else {\
        free(segment);\
    }\
}\
\
\
\
bbFlag CONCAT2(BB_NAME,_popFront)(\
    BB_NAME* deque, BB_CLASS* element) {\
    if (deque->count == 0) {\
        return bbEmpty;\
    }\
\
    U32 index = deque->begin++;\
\
    bbAssert(index < BB_SEGMENT_SIZE, "index out of bounds\n");\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->head;\
    deque->count--;\
    if (element != NULL) {\
        *element = segment->data[index];\
    }\
\
    if (deque->count == 0) {\
        CONCAT2(BB_NAME,_removeSegment)(deque, segment);\
        deque->begin = 0;\
        deque->end = 0;\
    } else if (deque->begin == BB_SEGMENT_SIZE) {\
        CONCAT2(BB_NAME,_removeSegment)(deque, segment);\
        deque->begin = 0;\
    }\
    return bbSuccess;\
}\
\
\
bbFlag CONCAT2(BB_NAME,_popBack)(\
    BB_NAME* deque, BB_CLASS* element) {\
    if (deque->count == 0) {\
        return bbEmpty;\
    }\
\
    U32 index = --deque->end;\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->tail;\
    deque->count--;\
    bbAssert(index < BB_SEGMENT_SIZE, "index out of bounds\n");\
\
    if (element != NULL) {\
        *element = segment->data[index];\
    }\
\
    if (deque->count == 0) {\
        CONCAT2(BB_NAME,_removeSegment)(deque, segment);\
        deque->begin = 0;\
        deque->end = 0;\
    } else if (deque->end == 0) {\
        CONCAT2(BB_NAME,_removeSegment)(deque, segment);\
        deque->end = BB_SEGMENT_SIZE;\
    }\
    return bbSuccess;\
}\
\
\
\
bbFlag CONCAT2(BB_NAME,_peekFront)(\
    const BB_NAME* deque, BB_CLASS* element) {\
    if (deque->count == 0) {return bbEmpty;}\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->head;\
    U32 index = deque->begin;\
    if (element != NULL) {\
        *element = segment->data[index];\
    }\
    return bbSuccess;\
\
}\
\
bbFlag CONCAT2(BB_NAME,_peekBack)(\
    const BB_NAME* deque, BB_CLASS* element) {\
    if (deque->count == 0) {return bbEmpty;}\
\
    CONCAT2(BB_NAME,_dequeSegment)* segment = deque->tail;\
    U32 index = deque->end-1;\
    if (element != NULL) {\
        *element = segment->data[index];\
    }\
    return bbSuccess;\
\
}\


#endif //BB_SEGMENTED_DEQUE_MACROS