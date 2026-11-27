#include "engine/actions/bbAction.h"

#include "engine/common/bbString.h"
#include "engine/storage/segmented/bbSegmentedList_sort.h"


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
        offsetof(bbAction, header.action_queue),
        121,
        NULL);

    if (flag != bbSuccess) {
        bbSegmentedPool_delete(&actions->pool);
        actions->pool = NULL;
        actions->action_queue = (bbSegmentedList){0};
        return flag;
    }

    flag = bbSegmentedList_init(
        &actions->temp_lifo,
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


bbFlag bbActions_request(
    bbActions* actions,
    bbNetwork* network,
    bbAction* action)
{
    if (action->header.status == bbAction_Local) {
        bbAction* action_element;
        bbSegmentedPool_alloc(actions->pool,NULL,(void**)&action_element);

        //*action_element = *action; doesnt work because it nukes
        // action_element->header.action_queue.list_id;

        action_element->header.sender = action->header.sender;
        action_element->header.collision = action->header.collision;
        action_element->header.created_tick = action->header.created_tick;
        action_element->header.act_tick = action->header.act_tick;
        action_element->header.status = action->header.status;
        action_element->header.type = action->header.type;
        bbStr_setStr(action_element->data.string, action->data.string, 128);


        bbSegmentedList_pushL(&actions->all_actions,action_element);
        bbSegmentedList_sortL(&actions->action_queue, bbAction_comparator,action_element);

        return bbSuccess;
    }

    bbNotImplemented();
}


I32 bbAction_comparator(
    void* object_A,
    void* object_B) {
    bbAction* action_A = (bbAction*)object_A;
    bbAction* action_B = (bbAction*)object_B;


    if (action_A->header.act_tick < action_B->header.act_tick) return 1;
    if (action_A->header.act_tick > action_B->header.act_tick) return  -1;
    if (action_A->header.collision < action_B->header.collision) return 1;
    if (action_A->header.collision > action_B->header.collision) return -1;
    if (action_A->header.sender < action_B->header.sender) return 1;
    if (action_A->header.sender > action_B->header.sender) return -1;

    bbWarning(false,"Failed to enforce total ordering on actions\n");
    return 0;
}
