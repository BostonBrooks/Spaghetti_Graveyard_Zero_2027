#ifndef BB_TREE_H
#define BB_TREE_H

#include "engine/common/bbHandle.h"
#include "engine/storage/segmented/bbSegmentedList.h"


typedef struct bbTree {

    bbSegmentedPool* pool;
    bbSegmentedList list;
    size_t node_offset;
    bbHandle root;
} bbTree;

typedef struct bbTreeNode {
    bbHandle parent;
    bbList_element peers;
    bbList_header children;
} bbTreeNode;

typedef struct bbTreeVisibility {
    bool visit_element;
    bool visit_children;
} bbTreeVisibility;

typedef bbFlag bbTreeFunction(bbTree* tree, void* element, void* cl);
typedef bbFlag bbTreeVisibilityFunction(bbTree* tree, void* element, void* cl);

bbFlag bbTree_init(bbTree* tree,
    bbSegmentedPool* pool,
    size_t node_offset);

bbFlag bbTreeNode_init(bbTreeNode* node);

bbFlag bbTree_setRoot(bbTree* tree,void* element);

/// add a new node as a daughter node to parent;
bbFlag bbTreeNode_setParent(bbTree* tree,
    void* element,
    void* parent);

///recursively search through nodes, filtering nodes with visibility_func
bbFlag bbTree_preOrderMapVisible(bbTree* tree,
                            void* element,
                            bbTreeFunction* my_func,
                            void* my_func_cl,
                            bbTreeVisibilityFunction* visibility_func,
                            void* visibility_func_cl);

///call bbTree_preOrderMapVisible with visibility_func = NULL
bbFlag bbTree_preOrderMap(bbTree* tree,
                            void* element,
                            bbTreeFunction* my_func,
                            void* cl);

#endif //BB_TREE_H