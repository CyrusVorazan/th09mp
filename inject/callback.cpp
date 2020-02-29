#include "callback.h"

#include "th09address.h"
#include "state.h"

#include <Windows.h>
#include <iostream>

namespace th09mp
{
   // Global state
   th09mp::address::Th9GlobalVer1_5* g = address::globals_ver1_5;

   // Saved state
   th09mp::address::Th9GlobalVer1_5* g_saved;

   // Utilities
   void PrintState()
   {
      SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });

      std::cout << (int)g->rng[0] << std::endl;
      std::cout << (int)g->rng[1] << std::endl;
   }

   void HandleInput()
   {
      if ((g->key_states[PlayerSide::Side_1P].keys & Keys::d) && !(g->key_states[PlayerSide::Side_1P].prev_keys & Keys::d))
      {
         CopyState(g, g_saved);
      }

      if ((g->key_states[PlayerSide::Side_1P].keys & Keys::r) && !(g->key_states[PlayerSide::Side_1P].prev_keys & Keys::r))
      {
         CopyState(g_saved, g);
      }
   }

   // Callbacks
   namespace callback
   {
      void OnFrameUpdate(void)
      {
         PrintState();
         HandleInput();
         //g->rng[0] = 0; //rand();
         //g->rng[1] = 0; //rand();
      }

      void OnGameStart(void)
      {
         //srand(1);
         // Initialize save state
         g_saved = new address::Th9GlobalVer1_5();
      }

      void OnGameEnd(void)
      {
         // Free save state
         // TODO: clean it up properly
         if (g_saved)
            delete g_saved;
      }
   }
}