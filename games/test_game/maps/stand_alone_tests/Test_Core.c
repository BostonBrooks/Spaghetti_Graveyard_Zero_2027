
#include "engine/core/bbCore.h"
#include "engine/core/bbCore_integrity.h"
#include "engine/core/bbInstruction_internal.h"
#include "engine/dependencies/CSFML.h"
static void test_core(void) {

    printf("Initializing core\n");
    bbCore core;
    bbCore_init(&core);
    bbInstructions_init(&core);
    bbInstructionTable_initVirtual(&core.instruction_table);
    bbFlag flag;

    sfClock *clock =sfClock_create();
    bbAssert(clock != NULL, "sfClock_create failed");


    for (U32 i = 0; i < 1000000; i++) {
        flag = bbCI_setTime(&core, i, bbInstructionSource_input,(bbHandle){0});
        bbAssert(flag == bbSuccess,"bbCI_setTime() returns  flag=%d\n", flag);
        flag = bbCore_react(&core);
        bbAssert(flag == bbSuccess,"bbCore_react() returns  flag=%d\n", flag);
    }


    bbAssert(core.simulation_time == 999999, "core left in unexpected state\n");

    sfTime time = sfClock_restart(clock);
    printf("Initial execution takes %lld microseconds\n", sfTime_asMicroseconds(time));

    flag = bbCore_checkIntegrity(&core);
    time = sfClock_restart(clock);
    printf("bbCore_checkIntegrity() takes %lld microseconds\n", sfTime_asMicroseconds(time));
    bbAssert(flag == bbSuccess,"bbCore_checkIntegrity() returns  flag=%d\n", flag);

    flag = bbCore_rewindUntil(&core, 0);


    bbAssert(core.simulation_time == 0, "core left in unexpected state\n");
    time = sfClock_restart(clock);
    printf("bbCore_rewindUntil() takes %lld microseconds\n", sfTime_asMicroseconds(time));
    bbAssert(flag == bbSuccess,"bbCore_rewindUntil() returns flag=%d\n", flag);

    flag = bbCore_checkIntegrity(&core);
    time = sfClock_restart(clock);
    printf("bbCore_checkIntegrity() takes %lld microseconds\n", sfTime_asMicroseconds(time));
    bbAssert(flag == bbSuccess,"bbCore_checkIntegrity() returns flag=%d\n", flag);

    flag = bbCore_react(&core);
    time = sfClock_restart(clock);
    printf("bbCore_react() takes %lld microseconds\n", sfTime_asMicroseconds(time));
    bbAssert(flag == bbSuccess,"bbCore_react() returns flag=%d\n", flag);

    flag = bbCore_checkIntegrity(&core);
    time = sfClock_restart(clock);
    printf("bbCore_checkIntegrity() takes %lld microseconds\n", sfTime_asMicroseconds(time));
    bbAssert(flag == bbSuccess,"bbCore_checkIntegrity() returns flag=%d\n", flag);

    bbAssert(core.active_instructions.count == 0, "instructions left on active stack\n");
    bbAssert(core.undo_instructions.count == 1000000, "wrong number of instructions on undo deque\n");
    bbAssert(core.redo_instructions.count == 1000000, "wrong number of instructions on redo deque\n");

    bbAssert(core.simulation_time == 999999, "core left in unexpected state\n");

    flag = bbCore_destroy(&core);
    bbAssert(flag == bbSuccess,"bbCore_destroy() failed\n");
    sfClock_destroy(clock);
    printf("test_core passed\n");
}


int main(void) {

    test_core();
    printf("All enabled core tests passed!\n");

    return 0;
}
