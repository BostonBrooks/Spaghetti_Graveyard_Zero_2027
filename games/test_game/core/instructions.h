#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "engine/actions/bbAction_instructions.h"
#include "engine/core/bbInstruction.h"

typedef enum
{
    bbI_setString = bbActionInstruction_numTypes,
    bbI_unsetString,

    bbVInstruction_numTypes
} bbVInstruction_type;


#endif //INSTRUCTIONS_H