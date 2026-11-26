#include "engine/actions/bbAction.h"


static bbFlag bbAction_alloc_fn(
    bbSegmentedPool* pool,
    void* object,
    void* cl
) {

    (void)pool;
    (void)cl;
    bbAction* action = object;

    action->header.action_queue = (bbList_element){0};
    action->header.all_actions = (bbList_element){0};
    return bbSuccess;
}
///Should be called after bbCore_init()
bbFlag bbActions_init(bbCore* core, bbActions* actions) {

    bbSegmentedPool* pool;

    bbFlag flag = bbSegmentedPool_new(
        &pool,
        sizeof(bbAction),
        alignof(bbAction),
        1024,
        123,
        bbAction_alloc_fn,
        NULL);

    if (flag != bbSuccess) {return flag;}

    actions->pool = pool;

    flag = bbSegmentedList_init(
        &actions->all_actions,
        actions->pool,
        offsetof(bbAction, header.all_actions),
        122,
        NULL);

    if (flag != bbSuccess) {
        bbSegmentedPool_delete(&actions->pool);
        actions->pool = NULL;
        return flag;
    }

    flag = bbSegmentedList_init(
        &actions->action_queue,
        actions->pool,
        offsetof(bbAction, header.all_actions),
        121,
        NULL);

    if (flag != bbSuccess) {
        bbSegmentedPool_delete(&actions->pool);
        actions->pool = NULL;
        actions->action_queue = (bbSegmentedList){0};
        return flag;
    }

    flag = bbSegmentedList_init(
        &actions->temp_fifo,
        actions->pool,
        offsetof(bbAction, header.action_queue),
        120,
        NULL);

    if (flag != bbSuccess) {
        bbSegmentedPool_delete(&actions->pool);
        actions->pool = NULL;
        actions->all_actions = (bbSegmentedList){0};
        actions->action_queue = (bbSegmentedList){0};
        return flag;
    }

    actions->action_functions = NULL;
    actions->num_action_functions = 0;
    actions->core = core;
    core->actions = actions;

    return bbSuccess;
}