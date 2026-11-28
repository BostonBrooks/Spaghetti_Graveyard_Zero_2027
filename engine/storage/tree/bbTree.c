#include "engine/storage/tree/bbTree.h"

#include <stdlib.h>

#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"

bbFlag bbTree_init(bbTree* tree,
                   bbSegmentedPool* pool,
                   size_t node_offset) {
    bbAssert(tree != NULL, "Passing a NULL tree\n");
    bbAssert(pool != NULL, "Passing a pool\n");

    bbFlag flag = bbSegmentedList_init(&tree->list,pool,node_offset+offsetof(bbTreeNode,peers),next_list_id(),NULL);

    if (flag != bbSuccess) {return flag;}
    tree->pool = pool;
    tree->node_offset = node_offset;
    tree->root = (bbHandle){0};

    return bbSuccess;


}

///Must only be called on fully detached, childless nodes
bbFlag bbTreeNode_init(bbTreeNode* node) {
    bbAssert(node != NULL, "Passing a NULL node\n");

    *node = (bbTreeNode){0};
    node->children.list_id = next_list_id();

    return bbSuccess;
}

bbFlag bbTree_setRoot(bbTree* tree,void* element) {
    bbAssert(tree != NULL, "Passing a NULL tree\n");

    if (element == NULL) {
        tree->root = (bbHandle){0};
        return bbSuccess;
    }

    bbAssert(tree->root.u64 == (U64){0}, "Tree already had root\n");

    bbHandle element_handle;
    bbFlag flag = bbSegmentedPool_reverseLookup(tree->pool, element, &element_handle);

    bbWarning(flag == bbSuccess, "pool lookup failed\n");
    if (flag != bbSuccess) {
        return flag;
    }
    bbTreeNode* node = (bbTreeNode*)((U8*)element + tree->node_offset);
    bbAssert(node->parent.u64 == (U64){0},
        "Root element already has parent\n");

    //flag = bbSegmentedList_pushR(&tree->list,element);

    tree->root = element_handle;

    return bbSuccess;
}

/// add a new node as a daughter node to parent;
bbFlag bbTreeNode_setParent(bbTree* tree,
    void* element,
    void* parent) {
    bbAssert(tree != NULL, "Passing a NULL tree\n");
    bbAssert(parent != NULL, "Passing a NULL parent\n");
    bbAssert(element != NULL, "Passing a NULL element\n");
    bbAssert(element != parent, "Node cannot be its own parent\n");
    bbFlag flag;
    bbHandle element_handle;
    flag = bbSegmentedPool_reverseLookup(tree->pool, element, &element_handle);
    bbAssert(flag == bbSuccess, "pool lookup failed\n");

    bbHandle parent_handle;
    flag = bbSegmentedPool_reverseLookup(tree->pool, parent, &parent_handle);
    bbAssert(flag == bbSuccess, "pool lookup failed\n");

    bbTreeNode* element_node = (bbTreeNode*)((U8*)element + tree->node_offset);
    bbTreeNode* parent_node = (bbTreeNode*)((U8*)parent + tree->node_offset);

    flag = bbSegmentedListHeader_pushR(&tree->list,&parent_node->children,element);
    bbAssert(flag == bbSuccess, "bbSegmentedListHeader_pushR failed\n");
    element_node->parent = parent_handle;

    return bbSuccess;

}

bbFlag bbTree_preOrderMap(bbTree* tree,
                            void* element,
                            bbTreeFunction* my_func,
                            void* cl) {

    bbAssert(tree != NULL, "Passing a NULL tree\n");
    bbAssert(my_func != NULL, "Passing a NULL my_func\n");


    if (element == NULL) {
        bbHandle root_handle = tree->root;
        if (root_handle.system.generation == BB_POOL_NULL_GENERATION) {
            return bbContinue;
        }
        bbFlag flag = bbSegmentedPool_lookup(tree->pool,root_handle,&element);
        bbAssert(flag == bbSuccess, "Pool lookup failed\n");
    }

    bbFlag flag = my_func(tree,element,cl);
    if (flag == bbBreak) {
        return bbBreak;
    }
    if (flag == bbSkipChildren) {
        return bbContinue;
    }

    bbTreeNode* node = (bbTreeNode*)((U8*)element + tree->node_offset);
    if (node->children.head.system.generation == BB_POOL_NULL_GENERATION
        || node->children.tail.system.generation == BB_POOL_NULL_GENERATION) {
        bbAssert(node->children.head.system.generation
            == node->children.tail.system.generation,
            "head/tail mismatch\n");
            return bbContinue;

    }
    void* child_element;

    flag = bbSegmentedListHeader_peekL(&tree->list, &node->children, (void**) &child_element);
    if (flag == bbEmpty) {
        return bbContinue;
    }
    bbAssert(flag == bbSuccess, "Peek list failed\n");
    while (1) {
        flag = bbTree_preOrderMap(tree,child_element,my_func,cl);
        switch (flag) {
            case bbBreak:
                return bbBreak;

            case bbContinue: {
                bbFlag flag2 = bbSegmentedListHeader_getNext(&tree->list,&node->children, child_element,&child_element);
                if (flag2 == bbEmpty) {
                    return bbContinue;
                }
            }
                break;

            case bbRepeat:
                break;

            default:
                bbWarning(false, "unknown flag\n");
                break;


        }
    }
}