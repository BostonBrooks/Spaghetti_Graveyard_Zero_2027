
#include "engine/actions/bbAction.h"
#include "engine/actions/bbAction_instructions.h"
#include "engine/core/bbCore.h"
#include "engine/core/bbCore_integrity.h"
#include "engine/core/bbInstruction_internal.h"
#include "games/test_game/code/set_string.h"

static void test_init_actions(void) {

    printf("Initializing core\n");
    bbCore core;
    bbCore_init(&core);
    //Initialise dispatch table for instruction callbacks
    bbInstructions_init(&core);
    //Initialise table for comparing undo and redo instructions
    bbInstructionTable_initVirtual(&core.instruction_table);

    void* ptr = bbI_checkActions_fn;
    void* ptr2 = core.instruction_functions[bbI_checkActions - bbInstruction_numTypes];
    bbAssert(ptr == ptr2, "Wrong value in function table lookup\n");


    bbActions actions;
    bbFlag flag = bbActions_init(&core, &actions);
    bbAssert(flag == bbSuccess,"bbActions_init() returns flag=%d\n", flag);
    bbActions_initFunctions(&actions);

    for (U32 i = 0; i < 7; i++) {
        flag = bbCI_setTime(&core, i, bbInstructionSource_input,(bbHandle){0});
        bbAssert(flag == bbSuccess,"bbCI_setTime() returns  flag=%d\n", flag);
        flag = bbCore_react(&core);
        bbAssert(flag == bbSuccess,"bbCore_react() returns  flag=%d\n", flag);
    }

    bbHere();

    bbAction_setString(&core,
                       "Expected TIME: 3",
                       0,
                       0,
                       3,
                       3);

    flag = bbCI_checkActions(
        &core,
        0,
        bbInstructionSource_internal,
        (bbHandle){0});

    bbCore_react(&core);


    bbHere();


    printf("test_init_actions passed\n");
}


int main(void) {

    test_init_actions();
    printf("All enabled action tests passed!\n");

    return 0;
}
