#ifndef BB_ACTION_H
#define BB_ACTION_H

#include "engine/common/bbIntTypes.h"
#include "engine/core/bbCore.h"
#include "engine/storage/segmented/bbSegmentedList.h"
#include "engine/timekeeping/bbTime.h"

typedef struct bbActions bbActions;

typedef enum
{
    bbActionType_setString,
} bbAction_type;

typedef enum
{
    bbAction_Local,
    bbAction_Speculative,
    bbAction_Wait,
    bbAction_Accept,
    bbAction_Modify,
    bbAction_Deny,
    bbAction_Unknown,
    bbAction_numStatus,
} bbAction_status;

typedef struct
{
    bbAction_type type;
    bbAction_status status;
    U32 sender;
    U32 collision;
    bbTime created_tick;
    bbTime act_tick;

    bbList_element action_queue;
    bbList_element all_actions;
} bbAction_header;

typedef union
{
    char string[128];
} bbAction_data;

typedef struct
{
    bbAction_header header;
    bbAction_data data;
} bbAction;

typedef  bbFlag bbAction_fn(bbCore* core, bbAction* action);

typedef struct bbActions
{
    bbSegmentedPool* pool;
    bbSegmentedList all_actions;
    bbSegmentedList action_queue;
    bbSegmentedList temp_lifo;
    bbAction_fn** action_functions;
    U32 num_action_functions;
    bbCore* core;

} bbActions;

///Should be called after bbCore_init()
bbFlag bbActions_init(bbCore* core, bbActions* actions);
///Supplied by the game developer, should be called after bbCore_init()
bbFlag bbActions_initFunctions(bbCore* core, bbActions* actions);

I32 bbAction_comparator(
    void* object_A,
    void* object_B);

#endif //BB_ACTION_H