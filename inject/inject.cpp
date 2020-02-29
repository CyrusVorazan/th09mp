#include "inject.h"

#include "th09address.h"
#include "callback.h"
#include "globals.h"

#include <Windows.h>

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
		   mov edx, [ecx+4]
		   inc edx
		   mov [ecx], ax
		   mov [ecx+4], edx
		   ret
	   }
   }

   SHORT __fastcall NetRNG(struct th09mp::raw_types::RNGSeed* seed) {
	   /*

		if(host) {
			SHORT rng = ZUNRNG(seed);
			RNGQueue.push(rng); // std::queue
			return rng;
		} else if (netplay_enabled) {
			while(RNGqueue.empty() {
				MSG msg = { };
				// GetMessage will not return until a message is part of the message queue
				// Therefore the game will freeze in place until a new RNG value is recieved by the network thread
				// The network thread will then send a message to the game's main thread every time the RNG queue gets a value

				::GetMessage(&msg, NULL, 0, 0);

				// Allow closing the game (I am not worrying about leftover memory and file handles because Windows clears those automatically)
				if(msg.message = WM_CLOSE || msg.message = WM_QUIT) {
					::ExitProcess(0);
				}
			}
			return RNGQueue.pop();
		} else {
			return ZUNRNG(seed)
		}
	   
	   */
	   return ZUNRNG(seed);
   }

   SHORT __declspec(naked) __fastcall OnRNGVer1_5(struct th09mp::raw_types::RNGSeed* seed) {
	   // Written in Assembly because full control over the registers is required
	   __asm {
			push ecx
			call NetRNG
			pop ecx
			ret
	   }
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
#pragma warning( default : 4309 )
}