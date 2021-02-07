#include "inject.h"

#include "th09address.h"
#include "th09types.h"
#include "callback.h"
#include "breakpoint.h"
#include "menu.h"
#include "network.h"

#include <Windows.h>

HHOOK m_hHook;

// TODO: Move this to some other file. I need a procedure that would send local menu inputs to the remote peer and apply remote peer's inputs locally.
LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//th09mp::network::ProcessPackets();

	LPMSG pMsg = reinterpret_cast<LPMSG>(lParam);

	if (nCode == HC_ACTION)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			//th09mp::network::SendMenuInput(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;

		case WM_KEYUP:
			//th09mp::network::SendMenuInput(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;

		case WM_CHAR:
			break;
		}
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

namespace th09mp
{
	void SetJumpTo(char* code, int from, int to)
	{
		*((int*)code) = to - from;
	}

	void WriteCode(char* inject_to, char* new_code, size_t size)
	{
		DWORD old_protect;
		::VirtualProtect(inject_to, size, PAGE_EXECUTE_READWRITE, &old_protect);
		::memcpy_s(inject_to, size, new_code, size);
	}

	int __declspec(naked) OnFrameUpdateVer1_5(void)
	{
		__asm {
			pushad;
			pushfd;
		}
		callback::OnFrameUpdate();
		__asm {
			popfd;
			popad;
			mov eax, 1;
			ret;
		}
	}

	void __declspec(naked) OnGameStartVer1_5(void)
	{
		__asm {
			pushad;
			pushfd;
		}
		callback::OnGameStart();
		__asm {
			popfd;
			popad;
			mov edx, 320h;
			ret;
		}
	}

	void __declspec(naked) OnGameEndVer1_5(void)
	{
		__asm {
			pushad;
			pushfd;
		}
		callback::OnGameEnd();
		__asm {
			popfd;
			popad;
			add ebx, 0E8h;
			ret;
		}
	}

	SHORT __declspec(naked) __fastcall ZUNRNG(struct th09mp::raw_types::RNGSeed* seed) {
		__asm {
			xor eax, eax
			mov ax, [ecx]
			xor ax, 0x9630
			xor edx, edx
			sub eax, 0x6553
			mov dx, ax
			shr dx, 0xE
			shl eax, 2
			add eax, edx
			mov edx, [ecx + 4]
			inc edx
			mov[ecx], ax
			mov[ecx + 4], edx
			ret
		}
	}

	SHORT __fastcall NetRNG(struct th09mp::raw_types::RNGSeed* seed)
	{
		return ZUNRNG(seed);
	}

	SHORT __declspec(naked) __fastcall OnRNGVer1_5(struct th09mp::raw_types::RNGSeed* seed)
	{
		// Written in Assembly because full control over the registers is required
		__asm {
			push ecx
			call NetRNG
			pop ecx
			ret
		}
	}
	void OnZUNNetplayInit(void) {

	}

	th09mp::BPCaveExec BPOnWindowCreated(struct x86Regs* Regs, void* BPParams)
	{
		// TODO: unregister the hook. Need to figure out when and where.
		m_hHook = SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, nullptr, GetCurrentThreadId());
		//ShowWindow((HWND)th09mp::address::globals_ver1_5->hwnd, SW_HIDE);
		//int virtualScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		//GetSystemMetrics(SM_CYVIRTUALSCREEN);
		//RECT rect;
		//GetWindowRect((HWND)th09mp::address::globals_ver1_5->hwnd, &rect);
		//SetWindowPos((HWND)th09mp::address::globals_ver1_5->hwnd, HWND_TOP, virtualScreenWidth - 2000, 0, rect.right - rect.left, rect.bottom - rect.top, 0);
		//SetWindowTextA((HWND)th09mp::address::globals_ver1_5->hwnd, "Touhou Kaeidzuka Multiplayer Patch");

		return th09mp::BPCaveExec::True;
	}

	//th09mp::BPCaveExec BPOnMainMenuFrameUpdate(struct x86Regs* Regs, void* BPParams)
	//{
	//    th09mp::address::globals_ver1_5->key_states[2].system_keys = th09mp::Keys::z;
	//    return th09mp::BPCaveExec::True;
	//}

	th09mp::BPCaveExec BPOnMatchModeSelectFrameUpdate(struct x86Regs* Regs, void* BPParams)
	{
		return th09mp::BPCaveExec::True;
	}

	th09mp::BPCaveExec BPOnCharacterSelectFrameUpdate(struct x86Regs* Regs, void* BPParams)
	{
		return th09mp::BPCaveExec::True;
	}

#pragma warning( disable : 4309 )
	void InjectOnFrameUpdate(void)
	{
		/**
		Rewrite
		.text:00420290                 retn
		To
		.text:00420290                 call OnFrameUpdate ;; return 1
		.text:00420295                 retn
		*/
		char* inject_to = th09mp::address::addr_on_frame_update.ver1_5;
		char code[] = {
			0xE8, 0, 0, 0, 0, // call OnFrameUpdate
			0xC3              // retn
		};
		SetJumpTo(code + 1, (int)(inject_to + 5), (int)OnFrameUpdateVer1_5);
		WriteCode(inject_to, code, sizeof(code));
	}

	void InjectOnReplayUpdate(void)
	{
		/**
		Rewrite
		retn
		To
		call OnFrameUpdate ;; return 1
		retn
		*/
		char* inject_to = th09mp::address::addr_on_replay_update.ver1_5;
		char code[] = {
			0xE8, 0, 0, 0, 0, // call OnFrameUpdate
			0xC3              // retn
		};
		SetJumpTo(code + 1, (int)(inject_to + 5), (int)OnFrameUpdateVer1_5);
		WriteCode(inject_to, code, sizeof(code));
	}

	void InjectOnGameStart(void)
	{
		/**
		Rewrite
		.text:0041B2C3                 mov     edx, 320h
		To
		.text:0041B2C3                 call OnGameStart
		*/
		char* inject_to = th09mp::address::addr_on_game_start.ver1_5;
		char code[] = {
			0xE8, 0, 0, 0, 0, // call OnGameStart
		};
		SetJumpTo(code + 1, (int)(inject_to + 5), (int)OnGameStartVer1_5);
		WriteCode(inject_to, code, sizeof(code));
	}

	void InjectOnGameEnd(void)
	{
		/**
		Rewrite
		.text:0041B9A2                 add     ebx, 0E8h
		To
		.text:0041B9A2                 call OnGameEnd
		.text:0041B9A7                 nop
		*/
		char* inject_to = th09mp::address::addr_on_game_end.ver1_5;
		char code[] = {
			0xE8, 0, 0, 0, 0, //call OnGameEnd
			0x90              //nop
		};
		SetJumpTo(code + 1, (int)(inject_to + 5), (int)OnGameEndVer1_5);
		WriteCode(inject_to, code, sizeof(code));
	}

	void InjectOnRNG(void) {
		/**
		Rewrite
		.text:0042AE20					xor eax,eax
		.text:0042AE22					mov ax,word ptr ds:[ecx]
		To
		.text:0042AE20					jmp OnRNG
		*/
		char* inject_to = th09mp::address::addr_on_rng.ver1_5;
		char code[] = {
			0xE9, 0, 0, 0, 0
		};
		SetJumpTo(code + 1, (int)(inject_to + 5), (int)OnRNGVer1_5);
		WriteCode(inject_to, code, sizeof(code));
	}

	void InjectOnZUNNetplay(void) {
		char* inject_to = th09mp::address::addr_zun_netplay_init.ver1_5;
		char code[] = {
			 0xC7, 0x05,  0xA4, 0x7E, 0x4A, 0x00,  0x00, 0x00, 0x00, 0x00,
			 0xC7, 0x06, 0x00, 0x00, 0x00, 0x00,
			 0x60,
			 0xE8,  0x00, 0x00, 0x00, 0x00,
			 0x61,
			 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
		};
		SetJumpTo(code + 18, (int)inject_to + 22, (int)OnZUNNetplayInit);
		WriteCode(inject_to, code, sizeof(code));
	}

	th09mp::BPCaveExec BPOnRenderDifficultySelect(struct x86Regs* Regs, void* BPParams)
	{
		//th09mp::address::globals_ver1_5->key_states[2].system_keys = Keys::down;

		//if (th09mp::address::globals_ver1_5->key_states[2].system_keys != th09mp::address::globals_ver1_5->key_states[2].prev_keys)
		network::SendSystemKeys(th09mp::address::globals_ver1_5->key_states[2].system_keys);
		network::ProcessPackets();
		return th09mp::BPCaveExec::True;
	}

	void InjectOnDifficultyMenu(void) {

		//BPCreate(0x429e23, BPOnMainMenuFrameUpdate, nullptr, 0, 5);
		//BPCreate(0x42a7d1, BPOnMatchModeSelectFrameUpdate, nullptr, 0, 5);

		BPCreate(0x42A45D, BPOnRenderDifficultySelect, nullptr, 0, 5);

		BPCreate(0x4289db, BPOnCharacterSelectFrameUpdate, nullptr, 0, 5);

		BPCreate(0x42D5D1, BPOnWindowCreated, nullptr, 0, 5); // or 0x42F00A

		BPCreate(0x42A69A, th09mp::BPOnShowDifficultyMenu, nullptr, 0, 5);
		//BPCreate(0x42A4AC, th09mp::BPOnDifficultyMenuCursorMove, nullptr, 0, 5);
	}
#pragma warning( default : 4309 )
}