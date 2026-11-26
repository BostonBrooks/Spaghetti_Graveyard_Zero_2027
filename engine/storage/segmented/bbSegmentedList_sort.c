#include "engine/storage/segmented/bbSegmentedList_sort.h"

#include <stdlib.h>

#include "bbSegmentedPool_internal.h"
#include "engine/common/bbTerminal.h"

///insert element into appropriate position in the list, searching from the left
bbFlag bbSegmentedList_sortL(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator,
    void* element)
{
    bbAssert(comparator != NULL, "Passing null comparator\n");
    bbAssert(element != NULL, "Passing null element\n");
    bbAssert(list != NULL, "Passing null list\n");

    bbList_header* header = list->list_pointer;
    bbHandle head = header->head;
    bbFlag flag;

    if (head.system.generation == BB_POOL_NULL_GENERATION)
    {
        flag = bbSegmentedList_pushL(list, element);
        return flag;
    }
    void* list_element;
    flag = bbSegmentedPool_lookup(list->pool,head, (void**)&list_element);
    if (flag != bbSuccess) return flag;

    while (comparator(element, list_element) >= 0)
    {
        flag = bbSegmentedList_getNext(
            list, list_element,
            (void**)&list_element);
        if (flag == bbEmpty) return(bbSegmentedList_pushR(list, element));

        if (flag != bbSuccess) return flag;
    }

    return bbSegmentedList_insertBefore(
        list,list_element,element);
}

///insert element into appropriate position in the list, searching from the right
bbFlag bbSegmentedList_sortR(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator,
    void* element)
{
    bbAssert(comparator != NULL, "Passing null comparator\n");
    bbAssert(element != NULL, "Passing null element\n");
    bbAssert(list != NULL, "Passing null list\n");

    bbList_header* header = list->list_pointer;
    bbHandle tail = header->tail;
    bbFlag flag;

    if (tail.system.generation == BB_POOL_NULL_GENERATION)
    {
        flag = bbSegmentedList_pushR(list, element);
        return flag;
    }
    void* list_element;
    flag = bbSegmentedPool_lookup(list->pool,tail, (void**)&list_element);
    if (flag != bbSuccess) return flag;

    while (comparator(element, list_element) < 0)
    {
        flag = bbSegmentedList_getPrev(
            list, list_element,
            (void**)&list_element);
        if (flag == bbEmpty) return(bbSegmentedList_pushL(list, element));

        if (flag != bbSuccess) return flag;
    }

    return bbSegmentedList_insertAfter(
        list,list_element,element);
}