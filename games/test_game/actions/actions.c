#include "engine/actions/bbAction.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbString.h"
#include "games/test_game/code/set_string.h"

bbFlag bbActions_initFunctions(bbActions* actions) {
    bbAction_fn** action_functions = calloc(
        bbActionType_num, sizeof(bbAction_fn*));
    bbAssert(action_functions != NULL, "Calloc failed\n");

    action_functions[bbActionType_setString] = bbA_setString_fn;
    actions->action_functions = action_functions;
    actions->num_action_functions = bbActionType_num;
    return bbSuccess;
}
