#include "callback.h"
#include "th09address.h"
#include "network.h"
#include <dinput.h>

namespace th09mp
{
    namespace network
    {
        extern std::map<unsigned int, unsigned short> inputStore;
        extern int latency = 90;
        extern bool isHost;
    }

    unsigned int frameNo = 0;

    // Global state
    th09mp::address::Th9GlobalVer1_5* g = address::globals_ver1_5;

    namespace input
    {
        std::map<int, Keys> dinputToPofvKeyMap = {
            {DIK_Z,           Keys::z},
            {DIK_X,           Keys::x},
            {DIK_LSHIFT,      Keys::shift},
            {DIK_LCONTROL,    Keys::ctrl},
            {DIK_ESCAPE,      Keys::esc},
            {DIK_RETURN,      Keys::enter},
            {DIK_P,           Keys::p},
            {DIK_Q,           Keys::q},
            {DIK_R,           Keys::r},
            {DIK_S,           Keys::s},
            {DIK_D,           Keys::d},
            {DIK_UPARROW,     Keys::up},
            {DIK_DOWNARROW,   Keys::down},
            {DIK_LEFTARROW,   Keys::left},
            {DIK_RIGHTARROW,  Keys::right}
        };
    
        std::map<unsigned int, unsigned short> localInputStore;

        void SetKeystate(PlayerSide side, unsigned short keys)
        {
            g->key_states[side].keys = keys;
            unsigned short changed_keys = g->key_states[side].keys ^ g->key_states[side].prev_keys;
            g->key_states[side].start_pushing_keys = changed_keys & g->key_states[side].keys;
            g->key_states[side].start_leaving_keys = changed_keys & ~g->key_states[side].keys;
        }
    
        unsigned short GetKeystate()
        {
            LPDIRECTINPUTDEVICE8 lpdiKeyboard = (LPDIRECTINPUTDEVICE8)g->dinput8_device;

            unsigned short keys = 0;

            if (lpdiKeyboard)
            {
                BYTE diKeys[256] = { 0 };

                lpdiKeyboard->Poll();
                if (lpdiKeyboard->GetDeviceState(256, diKeys) == DI_OK)
                {
                    for (const auto& kvp : dinputToPofvKeyMap)
                        if (diKeys[kvp.first] & 0x80)
                            keys |= kvp.second;
                }
            }

            return keys;
        }

        void Update()
        {
            // TODO: allow players to choose the side
            PlayerSide ourside = network::isHost ? PlayerSide::Side_1P : PlayerSide::Side_2P,
                theirside = network::isHost ? PlayerSide::Side_2P : PlayerSide::Side_1P;

            // Inputs collected on current frame will be actuated on both local and peer instances on "future frame" = "current frame" + "latency"

            // 0. Initialize "latency" empty frame inputs for the beginning
            if (localInputStore.size() < network::latency)
            {
                unsigned int tmpFrameNo = 1;
                while (localInputStore.size() < network::latency)
                    localInputStore[tmpFrameNo++] = 0;
            
                //network::SendInputData(localInputStore);
            }

            network::SendInputData(localInputStore);

            // 1. Add 1 to the number of the current frame
            frameNo++;

            // 2. If enemy input with the frame number of the current frame hasn't arrived yet, send local input data once again, and repeat the process after a small wait
            while (network::inputStore.find(frameNo) == network::inputStore.end())
            {
                network::ProcessPackets();
            }

            // 3. Obtain current input data and send it to the opponent with a frame number calculated as "current frame number + latency"
            unsigned short keys = GetKeystate();

            // 4. Save inputs generated during step 3 in memory
            localInputStore[frameNo + network::latency] = keys;
            network::SendInputData(localInputStore);

            // 5. Send P1 and P2's inputs for the current frame to PoFV
            SetKeystate(theirside, network::inputStore[frameNo]);
            SetKeystate(ourside, localInputStore[frameNo]);

            while (localInputStore.size() > network::latency * 2)
                localInputStore.erase(localInputStore.begin());
        }
    }

   // Callbacks
   namespace callback
   {
      void OnFrameUpdate(void)
      {
          input::Update();
      }

      void OnGameStart(void)
      {
      }

      void OnGameEnd(void)
      {
      }
   }
}