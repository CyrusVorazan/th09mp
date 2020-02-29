#pragma once

namespace th09mp
{
   // Code for injecting functions into th09's memory
   void InjectOnFrameUpdate(void);
   void InjectOnReplayUpdate(void);
   void InjectOnGameStart(void);
   void InjectOnGameEnd(void);
}