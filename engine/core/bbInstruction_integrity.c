#include "engine/core/bbInstruction_integrity.h"


bbFlag bbInstructionTable_initInternal(bbInstructionTable* table) {

    table->count = 0;
    table->forward = calloc(bbInstruction_numTypes, sizeof(*table->forward));
    table->rollback = calloc(bbInstruction_numTypes, sizeof(*table->rollback));

    if (table->forward == NULL || table->rollback == NULL) {
        free(table->forward);
        free(table->rollback);
        bbWarning(0==1, "Calloc failed\n");
        return bbFail;
    }
    table->count = bbInstruction_numTypes;


    for (I32 i = 0; i < bbInstruction_numTypes; i++)
    {
        table->forward[i] = WRONG_FUNCTION_TYPE;
        table->rollback[i] = WRONG_FUNCTION_TYPE;
    }

    table->forward[bbI_unsetString] = bbI_setString;
    table->rollback[bbI_setString] = bbI_unsetString;


    table->forward[bbI_unsetTime] = bbI_setTime;
    table->rollback[bbI_setTime] = bbI_unsetTime;

    return bbSuccess;
}