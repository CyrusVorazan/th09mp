/*
* th09mp: thcrap inspired breakpoint system
*/

#pragma once
#include <stdint.h>

namespace th09mp {
	const char BPEntry[] = {
		0x60,							// pushad
		0x9C,							// pushfd
		0x54,							// push esp
		0x68, 0x00, 0x00, 0x00, 0x00,	// push BPInfo
		0xE8, 0x00, 0x00, 0x00, 0x00,	// call BPProcess
		0x01, 0xC4,						// add esp, eax
		0x9D,							// popfd
		0x61,							// popad
		0xC3							// ret
	};

	enum class BPCaveExec : bool {
		False = false,
		True = true
	};

	typedef th09mp::BPCaveExec (*BPFunc_t)(struct x86Regs* Registers, void* BPParams);

	struct x86Regs {
		uint32_t flags;
		uint32_t edi;
		uint32_t esi;
		uint32_t ebp;
		uint32_t esp;
		uint32_t ebx;
		uint32_t edx;
		uint32_t ecx;
		uint32_t eax;
		char* Return; // In case you want to skip code using a breakpoint you can treat this like eip
	};
	
	struct BPInfo {
		BPFunc_t	BPFunc;
		uint32_t	BPAddr;
		void*		BPParams;
		uint32_t	Cavesize;
		char*		Cave;
	};

	struct BP {
		char EntryCode[sizeof(BPEntry)];
		char int3Padding[3];
		struct BPInfo BPInfo;
		char ParamsAndCodecave[];
	};

	void BPCreate(uint32_t addr, th09mp::BPFunc_t Func, void* BPParams, int BPParamsSize, int Cavesize);
}