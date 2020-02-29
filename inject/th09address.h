#pragma once
#include "th09types.h"

namespace th09mp {
    namespace address{
        
#pragma pack(push, 1)
        struct Th9GlobalVer1_5{
            struct raw_types::Board board[2];//4A7D94
            char unknown1[0x4A7E3C - 0x4A7D94 - sizeof(raw_types::Board)*2];
            struct raw_types::ExAttackContainer* ex_attack_container;//4A7E3C
            char unknown2[0x4A7E90 - 0x4A7E3C - sizeof(raw_types::ExAttackContainer*)];
            unsigned int round;//4A7E90
            char unknown3[4];
            unsigned int round_win[2];//4A7E98
            char unknown4[0x4A7EAC - 0x4A7E98 - sizeof(int)*2];
            unsigned int difficulty;//4A7EAC
            char unknown5[0x4A7EC4 - 0x4A7EAC - sizeof(int)];
            unsigned int play_status; // 0x1000: cut-in, 0x8: replay
            //char unknown6[0x4ACE18 - 0x4A7EC4 - sizeof(int)];
            char unknown6[0x4ACE0C - 0x4A7EC4 - sizeof(int)];
            unsigned int rng[2]; // 4ACE0C
            char unknown62[0x4ACE18 - 0x4ACE0C - sizeof(int)*2];
            struct raw_types::KeyState key_states[3];
            char unknown7[0x4B30B0 - 0x4ACE18 - sizeof(raw_types::KeyState)*3];
            int hwnd; //4B30B0
            char unknown8[0x4B3104 - 0x4B30B0 - sizeof(int)];
            int** d3d8;//4B3104
            int** d3d8_device;//4B3108
            char unknown9[0x4B353C - 0x4B3108 - sizeof(int**)];
            char charge_types[2];
            char unknown10[0x4B42D0 - 0x4B353C - sizeof(char)*2];
            struct raw_types::NetInfo* net_info;//4B42D0
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
        extern struct Th9GlobalVer1_5* const globals_ver1_5;
    }
}