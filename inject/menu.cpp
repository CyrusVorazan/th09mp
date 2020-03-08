#include "menu.h"

namespace th09mp {
	extern bool netplay_enabled;

	th09mp::BPCaveExec BPOnShowDifficultyMenu(struct x86Regs* Regs, void* BPParams) { 
		if (netplay_enabled) {
			Regs->esp += 8;
			return BPCaveExec::False;
		}

		return BPCaveExec::True;
	}

	th09mp::BPCaveExec BPOnDifficultyMenuCursorMove(struct x86Regs* Regs, void* BPParams) { 
		if (netplay_enabled) {
			Regs->esp += 8;
			return BPCaveExec::False;
		}
		return BPCaveExec::True; 
	}
}