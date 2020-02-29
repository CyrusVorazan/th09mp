#include "callback.h"

#include "th09address.h"

#include <Windows.h>
#include <iostream>

namespace th09mp
{
   // Global state
   th09mp::address::Th9GlobalVer1_5* g = address::globals_ver1_5;

   // Utilities
   void PrintState()
   {
      SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });

      // Print stuff here
   }

   void HandleInput()
   {
      if ((g->key_states[PlayerSide::Side_1P].keys & Keys::d) && !(g->key_states[PlayerSide::Side_1P].prev_keys & Keys::d))
      {
         
      }

      if ((g->key_states[PlayerSide::Side_1P].keys & Keys::r) && !(g->key_states[PlayerSide::Side_1P].prev_keys & Keys::r))
      {
         
      }
   }

   // Callbacks
   namespace callback
   {
      void OnFrameUpdate(void)
      {
         PrintState();
         HandleInput();
      }

      void OnGameStart(void)
      {

      }

      void OnGameEnd(void)
      {

      }
   }
}