
#ifndef BB_INSTRUCTION_H
#define BB_INSTRUCTION_H
#include <stdalign.h>

#include "engine/common/bbIntTypes.h"
#include "engine/common/bbFlag.h"
#include "engine/common/bbHandle.h"
#include "engine/storage/deque/bbSegmentedDeque_macros.h"
#include "engine/timekeeping/bbTime.h"
#include "engine/storage/deque/bbSegmentedDeque_iteratorMacros.h"
typedef struct bbCore bbCore;

typedef enum
{
    bbI_setTime,
    bbI_unsetTime,

    bbInstruction_numTypes
} bbInstruction_type;


typedef enum
{
    ///The instruction was added from inside the core
    bbInstructionSource_internal,
    ///The instruction was added in the MAIN thread
    bbInstructionSource_input,
    ///The instruction was added through the action queue
    bbInstructionSource_action,
    ///The instruction was called before the start of play
    bbInstructionSource_norewind,
} bbInstruction_source;

typedef struct {
    char string[128];
} bbInstructionData_maxPayload;

typedef union
{
    bbInstructionData_maxPayload maxPayload;
    bbTime time;
} bbInstruction_data;

typedef enum {
    bbI_data_empty,
    bbI_data_maxPayload,
    bbI_data_time,
    bbI_internal_data_count
} bbI_internal_data_type;

_Static_assert(sizeof(bbInstruction_data) == 128,
    "instruction_data should be 128 bytes");

typedef struct {
    bbInstruction_source source;
    U32 instruction_type;
    bbHandle redo_instruction;
} bbInstruction_header;

typedef struct {
    bbInstruction_header header;
    U32 data_type;
    alignas(8) bbInstruction_data data;
} bbInstruction;

typedef struct {
    U32 count;
    U32* forward;
    U32* rollback;
} bbInstructionTable;

typedef  bbFlag bbInstruction_fn(bbCore* core, bbInstruction* instruction);

DECLARE_DEQUE_HEADER(bbInstruction,bbInstruction, 1024)
DECLARE_DEQUE_ITERATOR_HEADER(bbInstruction,bbInstruction, 1024)


///The game designer provides this function to populate callback vtables
bbFlag bbInstructions_init(bbCore* core);

#endif //BB_INSTRUCTION_H