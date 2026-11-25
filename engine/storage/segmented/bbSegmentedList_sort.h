#ifndef BB_SEGMENTED_LIST_SORT_H
#define BB_SEGMENTED_LIST_SORT_H

#include "bbSegmentedList.h"

typedef bbFlag bbSegmentedList_comparator(
    bbSegmentedList* list,
    void* object_A,
    void* object_B,
    void* cl);


///insert element into appropriate position in the list, searching from the left
bbFlag bbSegmentedList_sortL(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator,
    void* element);

///insert element into appropriate position in the list, searching from the right
bbFlag bbSegmentedList_sortR(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator,
    void* element);

///keep the element in the same list, but move up or down to keep the list sorted
bbFlag bbSegmentedList_reposition(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator,
    void* element);

///Perform merge sort on entire list
bbFlag bbSegmentedList_sort(
    bbSegmentedList* list,
    bbSegmentedList_comparator* comparator);

#endif //BB_SEGMENTED_LIST_SORT_H