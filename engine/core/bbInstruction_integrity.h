#ifndef BB_INSTRUCTION_INTEGRITY_H
#define BB_INSTRUCTION_INTEGRITY_H

#include "bbInstruction.h"
#include "engine/common/bbFlag.h"

#define WRONG_FUNCTION_TYPE UINT32_MAX

bbFlag bbInstructionTable_initInternal(bbInstructionTable* table);

#endif //BB_INSTRUCTION_INTEGRITY_H