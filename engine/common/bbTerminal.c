#include "engine/common/bbTerminal.h"
#include <stdbool.h>
#include <stdint.h>

#include "engine/timekeeping/bbTime.h"

_Thread_local char* thread = "Unknown";
_Thread_local bool debug_off = false;
_Thread_local bool bbWarningsAreFatal = 1;
bbTime test_time;