#include "engine/actions/bbAction_instructions.h"

#include "bbAction.h"
#include "engine/core/bbCore_integrity.h"
#include "engine/storage/segmented/bbSegmentedList.h"
#include "games/test_game/code/set_string.h"


bbFlag bbCI_checkActions(
    bbCore* core,
    bbTime time,
    bbInstruction_source source,
    bbHandle action)
{
    bbInstruction instruction = {0};
    instruction.header.instruction_type = bbI_checkActions;
    instruction.header.source = source;
    instruction.header.redo_instruction = action;
    instruction.data_type = bbI_data_time;
    instruction.data.time = time;
    bbFlag flag = bbInstruction_dequePushFront(&core->active_instructions,&instruction);
    return flag;
}

bbFlag bbI_checkActions_fn(bbCore* core, bbInstruction* instruction) {



    bbAction* action;
    bbFlag flag;
    //bbSegmentedList_peekL();
    bbSegmentedList* queue = &core->actions->action_queue;
    bbSegmentedList* lifo = &core->actions->temp_lifo;

    flag = bbSegmentedList_peekL(queue, (void**)&action);
    if (flag == bbEmpty) goto BOOKKEEPING;
#ifndef NO_ROLLBACK
    if (action->header.act_tick < core->simulation_time) //or < the previous time this instruction was called?
    {
        bbCore_rewindUntil(core, action->header.act_tick-1);

        bbCore_react(core);

    }
#endif //NO_ROLLBACK

    flag = bbSegmentedList_peekL(queue, (void**)&action);
    while (flag == bbSuccess && action->header.act_tick <= core->simulation_time)
    {
        bbSegmentedList_popL(queue,(void**)&action);
        bbSegmentedList_pushL(lifo,(void*)action);

        flag = bbSegmentedList_peekL(queue, (void**)&action);
    }

    flag = bbSegmentedList_popL(lifo, (void**)&action);
    while (flag == bbSuccess) {
        bbHandle handle;
        bbSegmentedPool_reverseLookup(queue->pool, (void**)&action,&handle);

        if (action->header.type == bbActionType_setString)
        {
            bbCI_setString(core,action->data.string,bbInstructionSource_action,handle);
        }
    flag = bbSegmentedList_popL(lifo, (void**)&action);
    }
BOOKKEEPING:
    if (instruction->header.source == bbInstructionSource_input) {

        bbFlag flag = bbInstruction_dequePushFront(&core->redo_instructions, instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

        bbInstruction undo_instruction = {0};
        //Should bbI_uncheckActions be defined in the core or in the game
        undo_instruction.header.instruction_type = bbI_uncheckActions;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_empty;

        COMPARE_INSTRUCTIONS(&core->instruction_table, instruction, &undo_instruction);
        flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_internal) {

        bbInstruction undo_instruction = {0};
        //Should bbI_uncheckActions be defined in the core or in the game
        undo_instruction.header.instruction_type = bbI_uncheckActions;
        undo_instruction.header.source = instruction->header.source;
        undo_instruction.data_type = bbI_data_empty;

        bbFlag flag = bbInstruction_dequePushFront(&core->undo_instructions, &undo_instruction);
        bbAssert(flag == bbSuccess, "push failed in core, panic!\n");

    } else if (instruction->header.source == bbInstructionSource_action) {
                //the check actions function should not be called by a timed event


    }  else if (instruction->header.source == bbInstructionSource_norewind) {

    }

    return bbSuccess;

}
bbFlag bbI_uncheckActions_fn(bbCore* core, bbInstruction* instruction) {

    if (instruction->header.source == bbInstructionSource_internal)
    {
        return bbSuccess;
    } else if (instruction->header.source == bbInstructionSource_input)
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
                //the check actions function should not be called by a timed event
                bbNotImplemented()
                return bbSuccess;
            }
    bbNotHere();

    return bbSuccess;
}