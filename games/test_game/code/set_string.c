#include "games/test_game/code/set_string.h"
#include "games/test_game/core/instructions.h"
#include "engine/common/bbString.h"
#include "engine/core/bbCore_integrity.h"
char test_set_string[128];

bbFlag bbI_setString_fn(bbCore* core, bbInstruction* instruction) {


    if (instruction->header.source == bbInstructionSource_input) {

        bbFlag flag = bbInstruction_dequePushFront(&core->redo_instructions, instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetString;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_maxPayload;
        bbStr_setStr(
            undo_instruction.data.maxPayload.string,
            test_set_string,
            sizeof(instruction->data.maxPayload));

        COMPARE_INSTRUCTIONS(&core->instruction_table, instruction, &undo_instruction);
        flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_internal) {

        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetString;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_maxPayload;
        bbStr_setStr(
            undo_instruction.data.maxPayload.string,
            test_set_string,
            sizeof(instruction->data.maxPayload));

        bbFlag flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_action) {

        bbInstruction undo_instruction = {0};
        undo_instruction.header.instruction_type = bbI_unsetString;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_maxPayload;
        bbStr_setStr(
            undo_instruction.data.maxPayload.string,
            test_set_string,
            sizeof(instruction->data.maxPayload));

        undo_instruction.header.redo_instruction = instruction->header.redo_instruction;
        bbFlag flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");
    }  else if (instruction->header.source == bbInstructionSource_norewind) {

    }
    bbStr_setStr(
        test_set_string,
        instruction->data.maxPayload.string,
        sizeof(instruction->data.maxPayload));
    bbDebug("string = %s\n", instruction->data.maxPayload.string);
    return bbSuccess;

}
bbFlag bbI_unsetString_fn(bbCore* core, bbInstruction* instruction) {
    core->simulation_time = instruction->data.time;
    bbStr_setStr(
    test_set_string,
    instruction->data.maxPayload.string,
    sizeof(instruction->data.maxPayload));
    bbDebug("string = %s\n", instruction->data.maxPayload.string);


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



bbFlag bbCI_setString(bbCore* core, char* string, bbInstruction_source source, bbHandle action) {

    bbInstruction instruction = {0};
    instruction.header.instruction_type = bbI_setString;
    instruction.header.source = source;
    instruction.header.redo_instruction = action;
    instruction.data_type = bbI_data_maxPayload;
    bbStr_setStr(
        instruction.data.maxPayload.string,
        string,
        sizeof(instruction.data.maxPayload));

    bbFlag flag = bbInstruction_dequePushFront(&core->active_instructions,&instruction);
    return flag;
}
bbFlag bbCS_setString(bbCore* core, char* string, bbInstruction_source source, bbHandle action) {
    bbNotImplemented()

}
