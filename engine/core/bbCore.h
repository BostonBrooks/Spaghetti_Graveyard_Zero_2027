
#ifndef BB_CORE_H
#define BB_CORE_H
#include "engine/common/bbFlag.h"
#include "engine/core/bbInstruction.h"
#include "engine/timekeeping/bbTime.h"

typedef struct bbCore bbCore;
typedef struct bbActions bbActions;

struct bbCore {
    //callbacks for active, undo and redo instructions
    bbInstruction_fn** instruction_functions;
    //called when an undo instruction is discarded
    bbInstruction_fn** discard_functions;

    bbInstruction_deque active_instructions;
    bbInstruction_deque undo_instructions;
    bbInstruction_deque redo_instructions;

    //used to compare functions with their undo instructions
    bbInstructionTable instruction_table;

    bbTime rewind_until_time;
    bbTime simulation_time;
    bbTime actual_time;


    bbActions* actions;
    void* local_context; //per-map data
    void* global_context; //per-game data
};

bbFlag bbCore_init(bbCore* core);
bbFlag bbCore_destroy(bbCore* core);

bbFlag bbCore_react(bbCore* core);
bbFlag bbCore_rewindUntil(bbCore* core, bbTime time);

#endif //BB_CORE_H