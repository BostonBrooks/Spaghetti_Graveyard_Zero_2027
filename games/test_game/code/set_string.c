#include "games/test_game/code/set_string.h"
#include "games/test_game/core/instructions.h"
#include "engine/common/bbString.h"
char test_string[128];

bbFlag bbI_setString_fn(bbCore* core, bbInstruction* instruction) {

    bbDebug("string = %s\n", instruction->data.maxPayload.string);
    bbNotImplemented()
}
bbFlag bbI_unsetString_fn(bbCore* core, bbInstruction* instruction) {
    bbNotImplemented()
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
