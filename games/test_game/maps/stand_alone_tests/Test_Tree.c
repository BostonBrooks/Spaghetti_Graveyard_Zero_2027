#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "engine/common/bbIntTypes.h"
#include "engine/common/bbTerminal.h"
#include "engine/storage/segmented/bbSegmentedPool_internal.h"
#include "engine/storage/tree/bbTree.h"

typedef struct {
    U32 i;
    bbTreeNode node;
} test_element;

static bbFlag test_init_list_fn(
    bbSegmentedPool* pool,
    void* object,
    void* cl
) {
    (void)pool;
    (void)cl;
    test_element* test_element = object;
    bbTreeNode_init(&test_element->node);
    return bbSuccess;
}

typedef struct {
    U32 expected[16];
    U32 i;
} expected_cl;

bbFlag test_tree_function(bbTree* tree, void* element, void* cl) {
    test_element* test_element = element;
    expected_cl* expected = cl;
    (void) tree;
    //bbDebug("expected = %u, actual = %u\n", expected->expected[expected->i++], test_element->i);

    bbAssert(test_element->i == expected->expected[expected->i++],
        "Not expected value\n");
    return bbContinue;
}


static void test_init_tree(void) {
    bbFlag flag;
    bbSegmentedPool* pool;
    flag = bbSegmentedPool_new(
        &pool,
        sizeof(test_element),
        alignof(test_element),
        4,
        32,
        test_init_list_fn,
        NULL);

    bbAssert(flag == bbSuccess, "bbSegmentedPool_new failed");
    bbTree tree;
    flag = bbTree_init(&tree,pool,offsetof(test_element,node));
    bbAssert(flag == bbSuccess, "bbTree_init failed");

    test_element* elements[16];

    for (U32 i = 0; i < 16; i++) {
        flag = bbSegmentedPool_alloc(pool,NULL,(void**)&elements[i]);
        bbAssert(flag == bbSuccess, "bbSegmentedPool_alloc failed %u\n", flag);
        elements[i]->i = i;
    }

    flag = bbTree_setRoot(&tree,elements[0]);
    bbAssert(flag == bbSuccess, "bbTree_setRoot failed");

    for (U32 i = 1; i < 16; i++) {
        U32 parent = (i-1)/2;
        flag = bbTreeNode_setParent(&tree,elements[i],elements[parent]);
        bbAssert(flag == bbSuccess, "bbTree_setParent failed");
    }

    expected_cl expected;
    expected.i = 0;
    expected.expected[0] = 0;
    expected.expected[1] = 1;
    expected.expected[2] = 3;
    expected.expected[3] = 7;
    expected.expected[4] = 15;
    expected.expected[5] = 8;
    expected.expected[6] = 4;
    expected.expected[7] = 9;
    expected.expected[8] = 10;
    expected.expected[9] = 2;
    expected.expected[10] = 5;
    expected.expected[11] = 11;
    expected.expected[12] = 12;
    expected.expected[13] = 6;
    expected.expected[14] = 13;
    expected.expected[15] = 14;

    flag = bbTree_preOrderMap(&tree,NULL,test_tree_function,&expected);
    bbAssert(flag == bbContinue, "bbTree_preOrderMap failed");
}


int main(void) {
    test_init_tree();
    printf("All pre order map tests passed\n");
}