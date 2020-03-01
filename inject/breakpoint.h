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
	
	/*
	* Writing a breakpoint function:

	* The Registers parameter is a structure that describes all registers. When the function returns and you didn't write to this structure, all the registers will be the same when you get back to game code
	* If however you changed a member in the structure, then the register described by the member you wrote to will be different. The value of the register will be the value you wrote to the structure

	* The return value will determine if a copy of the code that got overwritten for the breakpoint call should be executed. The place in which that copy is will always jmp to the instruction after the breakpoint call
	  and NOP instructions if the cavesize is larger than 5

	* If you want a breakpoint to skip a bunch of code, you can write to the Return member. You can basically treat this like eip. But then make sure that you return BPCaveExec::False
	* If you change esp, the new value MUST be higher than the original esp. The purpouse of this is to be able to cleanup the stack if you put the breakpoint call on a function call
	*/

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
	
	/*
	* Create a thcrap style breakpoint
	* What this will do is create a call to code from this DLL, then return to gamecode with all registers exactly as they were, unless you changed them

	* addr: address in th09 code that will trigger the breakpoint
	* Func: Function that should be executed when the breakpoint hits.
	* BPParams: a structure that describes the parameters of the breakpoint. 
		Recommended ussage: declaring a structure type to hold information, passing a pointer to it to BPCreate (make sure it's allocated on the heap) 
		and in the breakpoint function, declare a pointer to the structure type and set the address it points to to the same one that BPParams points to
	* BPParamsSize: The size of the parameter strucutre
		If you did the recommended thing with BPParams, this parameter should be sizeof(BPParamsStructure)
	* Cavesize: This number needs to be at least 5; the amount of code bytes that should be overwritten at the place the breakpoint call should be placed
		Example: you want to put a breakpoint on a  "mov dword ptr ds:[esi], 0x49A964" instruction which is 6 bytes. The cavesize should be 6
		Example 2: You want to put a breakpoint on a "xor eax,eax" instruction. This instruction is 2 bytes. The next instruction is "and dword ptr ds:[ecx+0x80], eax" which is 6 bytes. The cavesize should be 8
	*/
	
	void BPCreate(uint32_t addr, th09mp::BPFunc_t Func, void* BPParams, int BPParamsSize, int Cavesize);
}