#pragma once

namespace th09mp
{
	void InjectCallbacks();

   // Code for injecting functions into th09's memory
   //void InjectOnFrameUpdate(void);
   //void InjectOnReplayUpdate(void);
   //void InjectOnGameStart(void);
   //void InjectOnGameEnd(void);
   //void InjectOnRNG(void);
   //void InjectOnZUNNetplay(void);

   void SetJumpTo(char* code, int from, int to);
   void WriteCode(char* inject_to, char* new_code, size_t size);
}