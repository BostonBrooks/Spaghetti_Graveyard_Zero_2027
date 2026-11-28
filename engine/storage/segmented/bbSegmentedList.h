#ifndef BB_SEGMENTED_LIST_H
#define BB_SEGMENTED_LIST_H

#include <stddef.h>

#include "engine/common/bbHandle.h"
#include "engine/common/bbFlag.h"
#include "engine/storage/segmented/bbSegmentedPool.h"
U32 next_list_id(void);

typedef struct {
    bbHandle head;
    bbHandle tail;
    U32 list_id;
} bbList_header;


typedef struct {
    bbHandle prev;
    bbHandle next;
    U32 list_id;
} bbList_element;

typedef struct {
    bbSegmentedPool* pool;

    bbList_header* list_pointer;
    bbList_header list;

    size_t offset_of;
} bbSegmentedList;

bbFlag bbSegmentedList_init(bbSegmentedList* list, bbSegmentedPool* pool, size_t offset, I32 list_id, bbList_header* list_pointer);

/**
 * @brief pushes an element to the right/tail of a list
 * @warning assumes the element is stored in list->pool
 */
bbFlag bbSegmentedList_pushR(bbSegmentedList* list, void* element);
/**
 * @brief pushes an element to the left/head of a list
 * @warning assumes the element is stored in list->pool
 */
bbFlag bbSegmentedList_pushL(bbSegmentedList* list, void* element);
bbFlag bbSegmentedList_popL(bbSegmentedList* list, void** element);
bbFlag bbSegmentedList_popR(bbSegmentedList* list, void** element);
bbFlag bbSegmentedList_peekL(bbSegmentedList* list, void** element);
bbFlag bbSegmentedList_peekR(bbSegmentedList* list, void** element);


bbFlag bbSegmentedListHeader_pushR(bbSegmentedList* list, bbList_header* header, void* element);
bbFlag bbSegmentedListHeader_peekL(bbSegmentedList* list, bbList_header* header, void** element);


bbFlag bbSegmentedList_remove(bbSegmentedList* list, void* element);

bbFlag bbSegmentedList_insertAfter(
    bbSegmentedList* list,
    void* existing_element,
    void* new_element);

bbFlag bbSegmentedList_insertBefore(
    bbSegmentedList* list,
    void* existing_element,
    void* new_element);

bbFlag bbSegmentedList_getNext(
    bbSegmentedList* list,
    void* this_element,
    void** next_element);

bbFlag bbSegmentedList_getPrev(
    bbSegmentedList* list,
    void* this_element,
    void** prev_element);

bbFlag bbSegmentedListHeader_getNext(
    bbSegmentedList* list,
    bbList_header* header,
    void* this_element,
    void** next_element);

#endif //BB_SEGMENTED_LIST_H