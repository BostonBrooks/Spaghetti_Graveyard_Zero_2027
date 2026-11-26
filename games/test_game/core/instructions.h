#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "engine/core/bbInstruction.h"

typedef enum
{
    bbI_setString = bbInstruction_numTypes,
    bbI_unsetString,

    bbVInstruction_numTypes
} bbVInstruction_type;


#endif //INSTRUCTIONS_H