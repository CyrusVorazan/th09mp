#include "breakpoint.h"
#include "inject.h"

#include <Windows.h>
#include <cstdio>

namespace th09mp {

	uint32_t BPProcess(struct th09mp::BPInfo* BPInfo, struct th09mp::x86Regs* Regs) {
		uint32_t esp_previous = Regs->esp;
		
		th09mp::BPCaveExec DoCaveExec = BPInfo->BPFunc(Regs, BPInfo->BPParams);
		
		if (DoCaveExec == BPCaveExec::True) {
			Regs->Return = BPInfo->Cave;
		}

		uint32_t esp_difference;
		if (esp_previous != Regs->esp) {
			esp_difference = Regs->esp - esp_previous;
			memmove((char*)Regs + esp_difference, Regs, sizeof(th09mp::x86Regs));
		}
		return esp_difference;
	}


	void BPCreate(uint32_t addr, th09mp::BPFunc_t Func, void* BPParams, int BPParamsSize, int Cavesize) {
		if (Cavesize < 5) {
			std::fprintf(stderr, "BP ERROR: Cavesize (%d) less than 5\n", Cavesize);
			return;
		}
		struct th09mp::BP* Breakpoint = static_cast<th09mp::BP*>(::VirtualAlloc(NULL, sizeof(th09mp::BP) + BPParamsSize + Cavesize + 9, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

		memcpy(&Breakpoint->ParamsAndCodecave[0], BPParams, BPParamsSize);
		Breakpoint->BPInfo.BPParams = &Breakpoint->ParamsAndCodecave[0];
		
		
		memcpy(&Breakpoint->EntryCode[0], th09mp::BPEntry, sizeof(th09mp::BPEntry));
		memset(&Breakpoint->int3Padding[0], 0xCC, 3);

		SetJumpTo(&Breakpoint->EntryCode[9], (int)&Breakpoint->EntryCode[13], (int)BPProcess);

		memset(&Breakpoint->ParamsAndCodecave[BPParamsSize], 0xCC, 3);
		Breakpoint->BPInfo.Cave = &Breakpoint->ParamsAndCodecave[BPParamsSize + 3];

		memset(&Breakpoint->BPInfo.Cave[Cavesize + 5], 0xCC, 3);
		memcpy(Breakpoint->BPInfo.Cave, (void*)addr, Cavesize);
		Breakpoint->BPInfo.Cave[Cavesize] = 0xE9;
		SetJumpTo(&Breakpoint->BPInfo.Cave[Cavesize + 1], (int)&Breakpoint->BPInfo.Cave[Cavesize + 5], addr + Cavesize);

		Breakpoint->BPInfo.BPFunc = Func;
		Breakpoint->BPInfo.BPAddr = addr;
		Breakpoint->BPInfo.Cavesize = Cavesize;
		
		char* BPCall = new char[Cavesize];
		BPCall[0] = 0xE8;
		SetJumpTo(&BPCall[1], addr + 5, (int)Breakpoint->EntryCode);
		for (int i = Cavesize - 5; i < Cavesize - 5; i++) {
			BPCall[5 + i] = 0x90;
		}
		WriteCode((char*)addr, BPCall, Cavesize);
		delete[] BPCall;

		return;
	}
}