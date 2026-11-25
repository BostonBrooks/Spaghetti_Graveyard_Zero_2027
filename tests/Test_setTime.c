
#include "engine/core/bbCore.h"
#include "engine/core/bbInstruction_internal.h"

static void test_set_time(void) {
    bbCore core;
    bbCore_init(&core);

    bbCI_setTime(&core, 16, bbInstructionSource_input,(bbHandle){0});
    bbCore_react(&core);

    bbCI_setTime(&core, 18, bbInstructionSource_input,(bbHandle){0});
    bbCore_react(&core);

    bbCI_setTime(&core, 21, bbInstructionSource_input,(bbHandle){0});
    bbCore_react(&core);

}


int main(void) {

    test_set_time();
    printf("All enabled set time tests passed!\n");

    return 0;
}
