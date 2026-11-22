#include "engine/storage/segmented/bbSegmentedList.h"

#include <stdlib.h>

#include "bbSegmentedPool_internal.h"
#include "engine/common/bbTerminal.h"

#define isNULL(A) ((A).system.generation == BB_POOL_NULL_GENERATION)

bbFlag bbSegmentedList_init(
    bbSegmentedList* list,
    bbSegmentedPool* pool,
    size_t offset,
    I32 list_id,
    bbList_header* list_pointer
    ) {


    bbAssert(list != NULL, "Null list pointer\n");
    bbAssert(pool != NULL, "Null pool pointer\n");
    bbAssert(list_id != 0, "Invalid list id\n");

    list->offset_of = offset;
    list->pool = pool;

    if (list_pointer == NULL) {
        list->list.head = (bbHandle){0};
        list->list.tail = (bbHandle){0};
        list->list.list_id = list_id;
        list->list_pointer = &list->list;

    } else {
        list->list_pointer = list_pointer;
    }

    return bbSuccess;
}

bbFlag bbSegmentedList_pushR(bbSegmentedList* list, void* element) {
    bbAssert(list != NULL, "Null list pointer\n");
    bbAssert(element != NULL, "Null element\n");

    bbList_element* link = (bbList_element*)((U8*)element+list->offset_of);

    bbAssert(link->list_id == 0, "Element already in a list\n");
    bbAssert(isNULL(link->prev), "Element prev not null\n");
    bbAssert(isNULL(link->next), "Element next not null\n");

    bbHandle handle;
    bbFlag flag = bbSegmentedPool_reverseLookup(
        list->pool,element,&handle);
    if (flag != bbSuccess) return flag;

    bbList_header* header = list->list_pointer;


    if(isNULL( header->head)){
        bbAssert(isNULL( header->tail), "head/tail mismatch\n");
        link->prev = handle;
        link->next = handle;
        header->head = handle;
        header->tail = handle;


    } else {
        void* head_object;
        void* tail_object;
        flag = bbSegmentedPool_lookup(list->pool, header->head, &head_object);
        if (flag != bbSuccess) return flag;
        flag = bbSegmentedPool_lookup(list->pool, header->tail, &tail_object);
        if (flag != bbSuccess) return flag;
        bbList_element* head_link = (bbList_element*)((U8*)head_object+list->offset_of);
        bbList_element* tail_link = (bbList_element*)((U8*)tail_object+list->offset_of);

        tail_link->next = handle;
        head_link->prev = handle;

        link->next = header->tail;
        link->prev = header->head;

        header->tail = handle;

    }

    link->list_id = header->list_id;
    return bbSuccess;
}


bbFlag bbSegmentedList_pushL(bbSegmentedList* list, void* element) {
    bbAssert(list != NULL, "Null list pointer\n");
    bbAssert(element != NULL, "Null element\n");

    bbList_element* link = (bbList_element*)((U8*)element+list->offset_of);

    bbAssert(link->list_id == 0, "Element already in a list\n");
    bbAssert(isNULL(link->prev), "Element prev not null\n");
    bbAssert(isNULL(link->next), "Element next not null\n");

    bbHandle handle;
    bbFlag flag = bbSegmentedPool_reverseLookup(
        list->pool,element,&handle);
    if (flag != bbSuccess) return flag;

    bbList_header* header = list->list_pointer;


    if(isNULL( header->head)){
        bbAssert(isNULL( header->tail), "head/tail mismatch\n");
        link->prev = handle;
        link->next = handle;
        header->head = handle;
        header->tail = handle;


    } else {
        void* head_object;
        void* tail_object;
        flag = bbSegmentedPool_lookup(list->pool, header->head, &head_object);
        if (flag != bbSuccess) return flag;
        flag = bbSegmentedPool_lookup(list->pool, header->tail, &tail_object);
        if (flag != bbSuccess) return flag;
        bbList_element* head_link = (bbList_element*)((U8*)head_object+list->offset_of);
        bbList_element* tail_link = (bbList_element*)((U8*)tail_object+list->offset_of);

        tail_link->next = handle;
        head_link->prev = handle;

        link->next = header->tail;
        link->prev = header->head;

        header->head = handle;

    }

    link->list_id = header->list_id;
    return bbSuccess;
}


bbFlag bbSegmentedList_popL(bbSegmentedList* list, void** element) {

    bbAssert(list != NULL, "Passing null list\n");
    bbAssert(element != NULL, "Passing null return reference\n");

    *element = NULL;
    bbList_header* header = list->list_pointer;


    //List Empty
    if (isNULL(header->head)){
        bbAssert(isNULL(header->tail), "head / tail mismatch\n");

        return bbNull;
    }

    void* head_object;
    void* tail_object;
    bbFlag flag = bbSegmentedPool_lookup(list->pool, header->head, &head_object);
    if (flag != bbSuccess) return flag;
    flag = bbSegmentedPool_lookup(list->pool, header->tail, &tail_object);
    if (flag != bbSuccess) return flag;
    bbList_element* head_link = (bbList_element*)((U8*)head_object+list->offset_of);
    bbList_element* tail_link = (bbList_element*)((U8*)tail_object+list->offset_of);

    bbAssert(head_link->list_id == header->list_id,
         "Head belongs to another list\n");

    if (head_object == tail_object) {
        // One element
        header->head = (bbHandle){0};
        header->tail = (bbHandle){0};


    } else {
        // More than one element

        bbHandle head_link_next = head_link->next;
        void* new_head_object;
        flag = bbSegmentedPool_lookup(list->pool,head_link_next,&new_head_object);

        if (flag!= bbSuccess) return flag;
        bbList_element* new_head_link = (bbList_element*)((U8*)new_head_object+list->offset_of);

        header->head = head_link_next;
        new_head_link->prev = header->tail;
        tail_link->next = header->head;
    }


    head_link->list_id = 0;
    head_link->prev = (bbHandle){0};
    head_link->next = (bbHandle){0};

    *element = head_object;

    return bbSuccess;
}


bbFlag bbSegmentedList_popR(bbSegmentedList* list, void** element) {

    bbAssert(list != NULL, "Passing null list\n");
    bbAssert(element != NULL, "Passing null return reference\n");

    *element = NULL;
    bbList_header* header = list->list_pointer;


    //List Empty
    if (isNULL(header->head)){
        bbAssert(isNULL(header->tail), "head / tail mismatch\n");

        return bbNull;
    }

    void* head_object;
    void* tail_object;
    bbFlag flag = bbSegmentedPool_lookup(list->pool, header->head, &head_object);
    if (flag != bbSuccess) return flag;
    flag = bbSegmentedPool_lookup(list->pool, header->tail, &tail_object);
    if (flag != bbSuccess) return flag;
    bbList_element* head_link = (bbList_element*)((U8*)head_object+list->offset_of);
    bbList_element* tail_link = (bbList_element*)((U8*)tail_object+list->offset_of);

    bbAssert(head_link->list_id == header->list_id,
         "Head belongs to another list\n");

    if (head_object == tail_object) {
        // One element
        header->head = (bbHandle){0};
        header->tail = (bbHandle){0};


    } else {
        // More than one element

        bbHandle tail_link_prev = tail_link->prev;
        void* new_tail_object;
        flag = bbSegmentedPool_lookup(list->pool,tail_link_prev,&new_tail_object);

        if (flag!= bbSuccess) return flag;
        bbList_element* new_tail_link = (bbList_element*)((U8*)new_tail_object+list->offset_of);

        header->tail = tail_link_prev;
        new_tail_link->next = header->head;
        head_link->prev = header->tail;
    }


    tail_link->list_id = 0;
    tail_link->prev = (bbHandle){0};
    tail_link->next = (bbHandle){0};

    *element = tail_object;

    return bbSuccess;
}