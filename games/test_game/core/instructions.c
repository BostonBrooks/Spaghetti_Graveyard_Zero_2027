#include "engine/core/bbInstruction.h"
#include "games/test_game/core/instructions.h"

#include "engine/core/bbCore.h"
#include "engine/core/bbCore_integrity.h"
#include "games/test_game/code/set_string.h"
///The game designer provides this function to populate callback vtables
bbFlag bbInstructions_init(bbCore* core) {
    I32 max_instructions = bbVInstruction_numTypes - bbInstruction_numTypes;
    bbInstruction_fn** functions = calloc( max_instructions, sizeof(bbInstruction_fn*));
    bbAssert(functions != NULL, "calloc failed\n");

    functions[bbI_setString -bbInstruction_numTypes] = bbI_setString_fn;
    functions[bbI_unsetString -bbInstruction_numTypes] = bbI_unsetString_fn;

    core->instruction_functions = functions;
    return bbSuccess;
}

bbFlag bbInstructionTable_initVirtual(bbInstructionTable* table) {

    U32* forward =  realloc(table->forward, sizeof(*table->forward)*bbVInstruction_numTypes);
    if (forward == NULL) {
        bbWarning(false, "Realloc failed\n");
        return bbFail;
    }
    table->forward = forward;

    U32* rollback =  realloc(table->rollback, sizeof(*table->rollback)*bbVInstruction_numTypes);
    if (rollback == NULL) {
        bbWarning(false, "Realloc failed\n");
        return bbFail;
    }
    table->rollback = rollback;

    for (I32 i = bbInstruction_numTypes; i < bbVInstruction_numTypes; i++) {
        forward[i] = WRONG_FUNCTION_TYPE;
        rollback[i] = WRONG_FUNCTION_TYPE;
    }

    table->forward[bbI_unsetString] = bbI_setString;
    table->rollback[bbI_setString] = bbI_unsetString;
    table->count = bbVInstruction_numTypes;

    return bbSuccess;

}