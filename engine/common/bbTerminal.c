#include "engine/common/bbTerminal.h"
#include <stdbool.h>

_Thread_local char* thread = "Unknown";
_Thread_local bool debug_off = false;