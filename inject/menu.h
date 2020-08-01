#pragma once
#include "breakpoint.h"

#include <string>

namespace th09mp {
	th09mp::BPCaveExec BPOnShowDifficultyMenu(struct x86Regs* Regs, void* BPParams);
	th09mp::BPCaveExec BPOnDifficultyMenuCursorMove(struct x86Regs* Regs, void* BPParams);
}