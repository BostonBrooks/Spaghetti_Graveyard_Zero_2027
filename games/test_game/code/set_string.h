#ifndef BB_SET_STRING_H
#define BB_SET_STRING_H
#include "engine/actions/bbAction.h"
#include "engine/common/bbFlag.h"
#include "engine/core/bbCore.h"
extern char test_set_string[128];

bbFlag bbAction_setString(void* Core,
                            char* string,
                            U32 sender,
                            U32 collision,
                            bbTime created_tick,
                            bbTime act_tick);

bbFlag bbA_setString_fn(bbCore* core, bbAction* action);
bbFlag bbI_setString_fn(bbCore* core, bbInstruction* instruction);
bbFlag bbI_unsetString_fn(bbCore* core, bbInstruction* instruction);
bbFlag bbCI_setString(bbCore* core, char* string, bbInstruction_source source, bbHandle action);
bbFlag bbCS_setString(bbCore* core, char* string, bbInstruction_source source, bbHandle action);

#endif