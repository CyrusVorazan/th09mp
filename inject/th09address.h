#pragma once
#include "th09types.h"

namespace th09mp {
    namespace address{
        
#pragma pack(push, 1)
        struct Th9GlobalVer1_5{
            struct raw_types::Board board[2];//4A7D94
            char unknown1[0x4A7E3C - 0x4A7D94 - sizeof(raw_types::Board)*2];
            struct raw_types::ExAttackContainer* ex_attack_container;//4A7E3C
            char unknown21[0x4A7E44 - 0x4A7E3C - sizeof(raw_types::ExAttackContainer*)];
            int rank;
            // 4A7E44 - Rank [1, 22]
            // 4A7E54 - Rank increase period [1, 0x7fffffff]
            // 4A7E58 - Maximum rank [1, 22]
            // 4A7E5C - Lily appearance counter [0, 0x7fffffff]
            char unknown2[0x4A7E90 - 0x4A7E44 - sizeof(int)];
            unsigned int round;//4A7E90
            char unknown3[4];
            unsigned int round_win[2];//4A7E98
            char unknown4[0x4A7EAC - 0x4A7E98 - sizeof(int)*2];
            unsigned int difficulty;//4A7EAC
            char unknown5[0x4A7EC4 - 0x4A7EAC - sizeof(int)];
            unsigned int play_status; // 0x1000: cut-in, 0x8: replay


            // 4A83E0 - 4AC869 : score data

            //char unknown6[0x4ACE18 - 0x4A7EC4 - sizeof(int)];

            char unknown6[0x4ACE0C - 0x4A7EC4 - sizeof(int)];
            unsigned int rng[2]; // 4ACE0C // 4ACE10 - rng count
            char unknown62[0x4ACE18 - 0x4ACE0C - sizeof(int)*2];

            struct raw_types::KeyState key_states[3]; // 0x4ACE18

            // 4acfc8 - some struct

            char unknown7[0x4B30B0 - 0x4ACE18 - sizeof(raw_types::KeyState) * 3];


            int hwnd; //4B30B0
            // 4B30B8 - bool isWindowActive
            // 4B30BC - bool isWindowInactive
            char unknown8[0x4B3104 - 0x4B30B0 - sizeof(int)];
            int** d3d8;//4B3104
            int** d3d8_device;//4B3108
            int** dinput8;//4B310C - IDirectInput8*
            int** dinput8_device;//4B3110 - LPDIRECTINPUTDEVICE8* (keyboard)
            //unknown
            //4B3174 - copy of 4B30B0 HWND


            //4b3450 - D3DPRESENT_PARAMETERS
            char unknown9[0x4B353C - 0x4B3110 - sizeof(int**)];
            
            //char unknown9[0x4B353C - 0x4B3108 - sizeof(int**)];
            char charge_types[2]; // 0x4b353c - 0x4b353d
            char unknown10[0x4B42D0 - 0x4B353C - sizeof(char)*2];
            struct raw_types::NetInfo* net_info;//4B42D0

            // 4dc544 - IDirect3DSurface8
        };
#pragma pack(pop)

        struct Address{
            char* const ver1_5;
        };

        extern struct raw_types::ExAttackFuncAddr ex_attack_func_addr_ver1_5;
        
        extern const struct Address addr_on_frame_update;
        extern const struct Address addr_on_replay_update;
        extern const struct Address addr_on_game_start;
        extern const struct Address addr_on_game_end;
		extern const struct Address addr_window_title;
		extern const struct Address addr_on_rng;
		extern const struct Address addr_zun_netplay_init;
        extern struct Th9GlobalVer1_5* const globals_ver1_5;
    }
}
