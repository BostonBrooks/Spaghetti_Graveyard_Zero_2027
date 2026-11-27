#ifndef BB_ACTION_INSTRUCTIONS_H
#define BB_ACTION_INSTRUCTIONS_H

#include "engine/common/bbFlag.h"
#include "engine/core/bbCore.h"
#include "engine/core/bbInstruction.h"

typedef enum
{
    bbI_checkActions = bbInstruction_numTypes,
    bbI_uncheckActions,

    bbActionInstruction_numTypes
} bbActionInstruction_type;

bbFlag bbCI_checkActions(bbCore* core, bbTime time, bbInstruction_source source, bbHandle action);
bbFlag bbI_checkActions_fn(bbCore* core, bbInstruction* instruction);
bbFlag bbI_uncheckActions_fn(bbCore* core, bbInstruction* instruction);

#endif //BB_ACTION_INSTRUCTIONS_H
