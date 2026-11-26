
#include "bbInstruction_internal.h"

#include "bbCore_integrity.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbHandle.h"
#include "engine/core/bbCore.h"
#include "engine/core/bbInstruction.h"


bbFlag bbCI_setTime(bbCore* core, bbTime time, bbInstruction_source source, bbHandle action) {

    bbInstruction instruction = {0};
    instruction.header.instruction_type = bbI_setTime;
    instruction.header.source = source;
    instruction.header.redo_instruction = action;
    instruction.data_type = bbI_data_time;
    instruction.data.time = time;

    bbFlag flag = bbInstruction_dequePushFront(&core->active_instructions,&instruction);
    return flag;
}

bbFlag bbI_setTime_fn(bbCore* core, bbInstruction* instruction) {
    if (instruction->header.source == bbInstructionSource_input) {

        bbFlag flag = bbInstruction_dequePushFront(&core->redo_instructions, instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetTime;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_time;
        undo_instruction.data.time = core->simulation_time;

        COMPARE_INSTRUCTIONS(&core->instruction_table, instruction, &undo_instruction);
        flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_internal) {


        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetTime;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_time;
        undo_instruction.data.time = core->simulation_time;
        //undo_instruction.header.redo_instruction = ?
        bbFlag flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_action) {
        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetTime;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_time;
        undo_instruction.data.time = core->simulation_time;
        undo_instruction.header.redo_instruction = instruction->header.redo_instruction;
        bbFlag flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");
    }  else if (instruction->header.source == bbInstructionSource_norewind) {

    }
    core->simulation_time = instruction->data.time;
    //bbDebug("Setting time to %ld\n", core->simulation_time);
    return bbSuccess;

}
bbFlag bbI_unsetTime_fn(bbCore* core, bbInstruction* instruction) {

    core->simulation_time = instruction->data.time;
    //bbDebug("Setting time back to %ld\n", core->simulation_time);


    if (instruction->header.source == bbInstructionSource_internal)
    {
        //bbVPool_free(core->instruction_pool, (void*)instruction);
        return bbSuccess;
    } else
    if (instruction->header.source == bbInstructionSource_input)
    {
        bbInstruction redo_instruction = {0};
        bbFlag flag;
        flag = bbInstruction_dequePopFront(&core->redo_instructions, &redo_instruction);
        bbAssert(flag == bbSuccess, "pop failed in core, panic!\n");
        COMPARE_INSTRUCTIONS(&core->instruction_table, &redo_instruction, instruction);
        flag = bbInstruction_dequePushFront(&core->active_instructions, &redo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");


        return bbSuccess;
    } else
    if (instruction->header.source == bbInstructionSource_action)
    {
        bbNotImplemented()
        return bbSuccess;
    }
    bbNotHere();

    return bbSuccess;
}
