#include "engine/core/bbCore.h"

#include "bbCore_integrity.h"
#include "engine/core/bbInstruction_internal.h"
bbFlag bbCore_init(bbCore* core) {
    core->instruction_functions = NULL;
    core->discard_functions = NULL;

    bbFlag flag;

    flag = bbInstruction_dequeInit(&core->active_instructions);
    if (flag != bbSuccess) {return flag;}
    flag = bbInstruction_dequeInit(&core->undo_instructions);
    if (flag != bbSuccess) {return flag;}
    flag = bbInstruction_dequeInit(&core->redo_instructions);
    if (flag != bbSuccess) {return flag;}

    //I might want to load built in instructions to the instruction table here

    flag = bbInstructionTable_initInternal(&core->instruction_table);
    if (flag != bbSuccess) {return flag;}

    core->rewind_until_time = 0;
    core->simulation_time = 0;
    core->actual_time = 0;
    core->local_context = NULL;
    core->global_context = NULL;
    core->actions = NULL;

    return bbSuccess;
}


bbFlag bbCore_destroy(bbCore* core)
{
    free(core->instruction_functions);
    core->instruction_functions = NULL;
    free(core->discard_functions);
    core->discard_functions = NULL;
    bbInstruction_dequeDestroy(&core->active_instructions);
    bbInstruction_dequeDestroy(&core->undo_instructions);
    bbInstruction_dequeDestroy(&core->redo_instructions);

    free(core->instruction_table.forward);
    core->instruction_table.forward = NULL;
    free(core->instruction_table.rollback);
    core->instruction_table.rollback = NULL;
    core->instruction_table.count = 0;

    return bbSuccess;

}

///The game designer provides this function to populate callback tbales
bbFlag bbCore_initCallbacks(bbCore* core);



bbFlag bbCore_react(bbCore* core) {
    bbFlag flag;
    while (1) {
        bbInstruction instruction;
        flag = bbInstruction_dequePeekFront(&core->active_instructions,&instruction);
        if (flag == bbEmpty) return bbSuccess;
        if (flag != bbSuccess) {return flag;}
        bbInstruction_dequePopFront(&core->active_instructions,NULL);

        if (instruction.header.instruction_type < bbI_internal_data_count) {
            switch (instruction.header.instruction_type) {
                case bbI_setTime:
                    bbI_setTime_fn(core,&instruction);
                    break;

                    //moved to /games/test_game
                //case bbI_setString:
                    //bbI_setString_fn(core,&instruction);
                    //break;
                default:
                    bbAssert(0==1,"Unknown instruction type %d\n", instruction.header.instruction_type);
            }
        } else {
            bbInstruction_fn* function =
                core->instruction_functions[instruction.header.instruction_type - bbInstruction_numTypes];
                bbAssert(function != NULL, "instruction function not found, panic\n");
                function(core,&instruction);
        }
    }
}


bbFlag bbCore_rewindUntil(bbCore* core, bbTime time) {
    bbFlag flag;
    while (core->simulation_time > time) {

        bbInstruction instruction;
        flag = bbInstruction_dequePeekFront(&core->undo_instructions,&instruction);
        if (flag == bbEmpty) return bbSuccess;
        if (flag != bbSuccess) {return flag;}
        bbInstruction_dequePopFront(&core->undo_instructions,NULL);
        if (instruction.header.instruction_type < bbI_internal_data_count) {
            switch (instruction.header.instruction_type) {
                case bbI_unsetTime:
                    bbI_unsetTime_fn(core,&instruction);
                    break;

                    //moved to /games/test_game
                //case bbI_unsetString:
                    //bbI_unsetString_fn(core,&instruction);
                    //break;
                default:
                    bbAssert(0==1,"Unknown instruction type %d\n", instruction.header.instruction_type);
            }
        } else {
            bbInstruction_fn* function =
                core->instruction_functions[instruction.header.instruction_type - bbInstruction_numTypes];
            bbAssert(function != NULL, "instruction function not found, panic\n");
            function(core,&instruction);
        }
    }
    return bbSuccess;
}