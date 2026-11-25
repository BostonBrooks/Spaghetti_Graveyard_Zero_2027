#ifndef BB_INSTRUCTION_INTEGRITY_H
#define BB_INSTRUCTION_INTEGRITY_H

#include "bbInstruction.h"
#include "engine/common/bbFlag.h"

#define WRONG_FUNCTION_TYPE UINT32_MAX

bbFlag bbInstructionTable_initInternal(bbInstructionTable* table);

bbFlag bbCore_compareInstructions(
    const bbInstructionTable* table,
    const bbInstruction* forward,
    const bbInstruction* rollback);

bbFlag bbCore_checkIntegrity(bbCore* core);

#define COMPARE_INSTRUCTIONS(table, forward, rollback)\
    bbCore_compareInstructions(table, forward, rollback)


#endif //BB_INSTRUCTION_INTEGRITY_H