#ifndef BB_INSTRUCTION_BUILTIN_H
#define BB_INSTRUCTION_BUILTIN_H
#include "bbCore.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbHandle.h"

extern char test_string[128];


bbFlag bbCI_setTime(bbCore* core, bbTime time, bbInstruction_source source, bbHandle action);
bbFlag bbI_setTime_fn(bbCore* core, bbInstruction* instruction);
bbFlag bbI_unsetTime_fn(bbCore* core, bbInstruction* instruction);



#endif //BB_INSTRUCTION_BUILTIN_H