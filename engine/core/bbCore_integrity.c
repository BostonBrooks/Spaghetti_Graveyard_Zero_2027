#include "engine/core/bbCore_integrity.h"

#include "bbCore.h"
#include "engine/storage/deque/bbSegmentedDeque_iterator.h"


bbFlag bbInstructionTable_initInternal(bbInstructionTable* table) {

    table->count = 0;
    table->forward = calloc(bbInstruction_numTypes, sizeof(*table->forward));
    table->rollback = calloc(bbInstruction_numTypes, sizeof(*table->rollback));

    if (table->forward == NULL || table->rollback == NULL) {
        free(table->forward);
        free(table->rollback);
        bbWarning(0==1, "Calloc failed\n");
        return bbFail;
    }
    table->count = bbInstruction_numTypes;


    for (I32 i = 0; i < bbInstruction_numTypes; i++)
    {
        table->forward[i] = WRONG_FUNCTION_TYPE;
        table->rollback[i] = WRONG_FUNCTION_TYPE;
    }

    table->forward[bbI_unsetString] = bbI_setString;
    table->rollback[bbI_setString] = bbI_unsetString;


    table->forward[bbI_unsetTime] = bbI_setTime;
    table->rollback[bbI_setTime] = bbI_unsetTime;

    return bbSuccess;
}

bbFlag bbCore_compareInstructions(
    const bbInstructionTable* table,
    const bbInstruction* forward,
    const bbInstruction* rollback)
{
    U32 forward_int = forward->header.instruction_type;
    U32 rollback_int = rollback->header.instruction_type;

    bbAssert(forward_int < table->count && rollback_int < table->count,
        "instruction_type out of range, panic");

    bbAssert(table->forward[rollback_int] == forward_int,
        "rollback -> forward instruction type mismatch, panic\n");

    bbAssert(table->rollback[forward_int] == rollback_int,
        "forward -> rollback instruction type mismatch, panic\n");

    return bbSuccess;
}


bbFlag bbCore_checkIntegrity(bbCore* core) {
    bbInstructionTable* table = &core->instruction_table;
    bbInstruction_deque *undo = &core->undo_instructions;
    bbInstruction_deque *redo = &core->redo_instructions;
    bbNotImplemented();
}