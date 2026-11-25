
#ifndef BB_CORE_H
#define BB_CORE_H
#include "engine/common/bbFlag.h"
#include "engine/core/bbInstruction.h"
#include "engine/timekeeping/bbTime.h"

typedef struct bbCore bbCore;


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



    void* local_context; //per-map data
    void* global_context; //per-game data
};

bbFlag bbCore_init(bbCore* core);

///The game designer provides this function to populate callback vtables
bbFlag bbCore_initCallbacks(bbCore* core);

bbFlag bbCore_react(bbCore* core);

#endif //BB_CORE_H