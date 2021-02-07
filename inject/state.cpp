#include "state.h"

#include <Windows.h>

namespace th09mp
{
    void SetInputState(PlayerSide side, unsigned short newKeys, bool setSystemKeys, bool addKeys)
    {
        th09mp::address::Th9GlobalVer1_5* g = address::globals_ver1_5;

        unsigned short& keys = setSystemKeys ? g->key_states[side].system_keys : g->key_states[side].keys;
        keys = addKeys ? keys | newKeys : newKeys;
        unsigned short changed_keys = keys ^ g->key_states[side].prev_keys;
        g->key_states[side].start_pushing_keys = changed_keys & keys;
        g->key_states[side].start_leaving_keys = changed_keys & ~keys;
    }

    // WARNING: this code is experimental and unfinished and all sorts of bad. It's used only to test if the game has deterministic state.

   void CopyVector2D(raw_types::Vector2D& dst, raw_types::Vector2D src)
   {
      dst.x = src.x;
      dst.y = src.y;
   }

   void CopyVector3D(raw_types::Vector3D& dst, raw_types::Vector3D src)
   {
      dst.x = src.x;
      dst.y = src.y;
      dst.z = src.z;
   }

   void CopyRect2D(raw_types::Rect2D& dst, raw_types::Rect2D src)
   {
      CopyVector2D(dst.ld, src.ld);
      CopyVector2D(dst.ru, src.ru);
   }

   void CopyRect3D(raw_types::Rect3D& dst, raw_types::Rect3D src)
   {
      CopyVector3D(dst.ld, src.ld);
      CopyVector3D(dst.ru, src.ru);
   }

   void CopySize2D(raw_types::Size2D& dst, raw_types::Size2D src)
   {
      dst.width = src.width;
      dst.height = src.height;
   }

   void CopyBoard(raw_types::Board src, raw_types::Board& dst)
   {
      // Player
      if (!dst.player)
         dst.player = new raw_types::Player();

      memcpy(dst.player->unknown1, src.player->unknown1, sizeof(src.player->unknown1));
      dst.player->is_ai = src.player->is_ai;
      memcpy(dst.player->unknown2, src.player->unknown2, sizeof(src.player->unknown2));
      dst.player->card_attack_level = src.player->card_attack_level;
      dst.player->boss_card_attack_level = src.player->boss_card_attack_level;
      dst.player->life = src.player->life;
      memcpy(dst.player->unknown3, src.player->unknown3, sizeof(src.player->unknown3));
      CopyVector3D(dst.player->position, src.player->position);
      memcpy(dst.player->unknown4, src.player->unknown4, sizeof(src.player->unknown4));
      CopyRect3D(dst.player->rect1, src.player->rect1);
      CopyRect3D(dst.player->rect2, src.player->rect2);
      CopyRect3D(dst.player->rect3, src.player->rect3);
      CopyVector3D(dst.player->rect_size1, src.player->rect_size1);
      CopyVector3D(dst.player->rect_size2, src.player->rect_size2);
      CopyVector3D(dst.player->rect_size3, src.player->rect_size3);
      memcpy(dst.player->unknown5, src.player->unknown5, sizeof(src.player->unknown5));

      // Player feature
      if (!dst.player->feature)
         dst.player->feature = new raw_types::PlayerFeature();
      memcpy(dst.player->feature->unknown1, src.player->feature->unknown1, sizeof(src.player->feature->unknown1));
      dst.player->feature->radius = src.player->feature->radius;
      memcpy(dst.player->feature->unknown2, src.player->feature->unknown2, sizeof(src.player->feature->unknown2));
      dst.player->feature->item_size = src.player->feature->item_size;
      dst.player->feature->speed_fast = src.player->feature->speed_fast;
      dst.player->feature->speed_slow = src.player->feature->speed_slow;
      dst.player->feature->speed_fast_div_sqrt2 = src.player->feature->speed_fast_div_sqrt2;
      dst.player->feature->speed_slow_div_sqrt2 = src.player->feature->speed_slow_div_sqrt2;
      dst.player->feature->charge_speed = src.player->feature->charge_speed;

      // Player
      memcpy(dst.player->unknown6, src.player->unknown6, sizeof(src.player->unknown6));
      dst.player->charge_current = src.player->charge_current;
      dst.player->charge_max = src.player->charge_max;
      memcpy(dst.player->unknown7, src.player->unknown7, sizeof(src.player->unknown7));
      dst.player->combo = src.player->combo;
      memcpy(dst.player->unknown8, src.player->unknown8, sizeof(src.player->unknown8));
      dst.player->spell_point = src.player->spell_point;
      memcpy(dst.player->unknown9, src.player->unknown9, sizeof(src.player->unknown9));
      for (int i = 0; i < 4; i++) // Item
      {
         dst.player->items[i].type = src.player->items[i].type;
         CopyVector3D(dst.player->items[i].position, src.player->items[i].position);
         CopyVector3D(dst.player->items[i].velocity, src.player->items[i].velocity);
         dst.player->items[i].enabled = src.player->items[i].enabled;
         memcpy(dst.player->items[i].unknown1, src.player->items[i].unknown1, sizeof(src.player->items[i].unknown1));
      }

      // Bullet container
      if (!dst.bullet_container)
         dst.bullet_container = new raw_types::BulletContainer();

      memcpy(dst.bullet_container->unknown1, src.bullet_container->unknown1, sizeof(src.bullet_container->unknown1));
      for (int i = 0; i < 536; i++)
      {
         memcpy(dst.bullet_container->bullets[i].unknown1, src.bullet_container->bullets[i].unknown1, sizeof(src.bullet_container->bullets[i].unknown1));
         dst.bullet_container->bullets[i].size.height = src.bullet_container->bullets[i].size.height;
         dst.bullet_container->bullets[i].size.width = src.bullet_container->bullets[i].size.width;
         memcpy(dst.bullet_container->bullets[i].unknown2, src.bullet_container->bullets[i].unknown2, sizeof(src.bullet_container->bullets[i].unknown2));
         CopyVector3D(dst.bullet_container->bullets[i].position, src.bullet_container->bullets[i].position);
         CopyVector3D(dst.bullet_container->bullets[i].velocity, src.bullet_container->bullets[i].velocity);
         memcpy(dst.bullet_container->bullets[i].unknown3, src.bullet_container->bullets[i].unknown3, sizeof(src.bullet_container->bullets[i].unknown3));
         dst.bullet_container->bullets[i].status = src.bullet_container->bullets[i].status;
         memcpy(dst.bullet_container->bullets[i].unknown4, src.bullet_container->bullets[i].unknown4, sizeof(src.bullet_container->bullets[i].unknown4));
         dst.bullet_container->bullets[i].unknown_status = src.bullet_container->bullets[i].unknown_status;
         dst.bullet_container->bullets[i].bullet_type = src.bullet_container->bullets[i].bullet_type;
         memcpy(dst.bullet_container->bullets[i].unknown6, src.bullet_container->bullets[i].unknown6, sizeof(src.bullet_container->bullets[i].unknown6));
      }
      memcpy(dst.bullet_container->unknown2, src.bullet_container->unknown2, sizeof(src.bullet_container->unknown2));
      for (int i = 0; i < 48; i++)
      {
         memcpy(dst.bullet_container->lasers[i].unknown1, src.bullet_container->lasers[i].unknown1, sizeof(src.bullet_container->lasers[i].unknown1));
         CopyVector3D(dst.bullet_container->lasers[i].position, src.bullet_container->lasers[i].position);
         dst.bullet_container->lasers[i].angle = src.bullet_container->lasers[i].angle;
         dst.bullet_container->lasers[i].length_1 = src.bullet_container->lasers[i].length_1;
         dst.bullet_container->lasers[i].length_2 = src.bullet_container->lasers[i].length_2;
         dst.bullet_container->lasers[i].unknown2 = src.bullet_container->lasers[i].unknown2;
         dst.bullet_container->lasers[i].thickness = src.bullet_container->lasers[i].thickness;
         memcpy(dst.bullet_container->lasers[i].unknown3, src.bullet_container->lasers[i].unknown3, sizeof(src.bullet_container->lasers[i].unknown3));
         dst.bullet_container->lasers[i].enabled = src.bullet_container->lasers[i].enabled;
         memcpy(dst.bullet_container->lasers[i].unknown4, src.bullet_container->lasers[i].unknown4, sizeof(src.bullet_container->lasers[i].unknown4));
      }

      // Unknown 1
      memcpy(dst.unknown1, src.unknown1, sizeof(src.unknown1));

      // Enemy container
      if (!dst.enemy_container)
         dst.enemy_container = new raw_types::EnemyContainer();

      memcpy(dst.enemy_container->unknown1, src.enemy_container->unknown1, sizeof(src.enemy_container->unknown1));

      for (int i = 0; i < 128; i++)
      {
         memcpy(dst.enemy_container->enemies[i].unknown1, src.enemy_container->enemies[i].unknown1, sizeof(src.enemy_container->enemies[i].unknown1));
         CopyVector3D(dst.enemy_container->enemies[i].position, src.enemy_container->enemies[i].position);
         memcpy(dst.enemy_container->enemies[i].unknown2, src.enemy_container->enemies[i].unknown2, sizeof(src.enemy_container->enemies[i].unknown2));
         CopyVector3D(dst.enemy_container->enemies[i].velocity, src.enemy_container->enemies[i].velocity);
         CopyVector3D(dst.enemy_container->enemies[i].velocity2, src.enemy_container->enemies[i].velocity2);
         memcpy(dst.enemy_container->enemies[i].unknown3, src.enemy_container->enemies[i].unknown3, sizeof(src.enemy_container->enemies[i].unknown3));
         CopySize2D(dst.enemy_container->enemies[i].size, src.enemy_container->enemies[i].size);
         memcpy(dst.enemy_container->enemies[i].unknown4, src.enemy_container->enemies[i].unknown4, sizeof(src.enemy_container->enemies[i].unknown4));
         CopyVector3D(dst.enemy_container->enemies[i].position_copy, src.enemy_container->enemies[i].position_copy);
         memcpy(dst.enemy_container->enemies[i].unknown5, src.enemy_container->enemies[i].unknown5, sizeof(src.enemy_container->enemies[i].unknown5));
         dst.enemy_container->enemies[i].status = src.enemy_container->enemies[i].status;
         dst.enemy_container->enemies[i].status2 = src.enemy_container->enemies[i].status2;
         memcpy(dst.enemy_container->enemies[i].unknown6, src.enemy_container->enemies[i].unknown6, sizeof(src.enemy_container->enemies[i].unknown6));
         CopyRect2D(dst.enemy_container->enemies[i].appear_area, src.enemy_container->enemies[i].appear_area);
         memcpy(dst.enemy_container->enemies[i].unknown7, src.enemy_container->enemies[i].unknown7, sizeof(src.enemy_container->enemies[i].unknown7));
      }

      // Unknown 2
      memcpy(dst.unknown2, src.unknown2, sizeof(src.unknown2));

      // Score
      if (!dst.score)
         dst.score = new raw_types::Score();

      memcpy(dst.score->unknown1, src.score->unknown1, sizeof(src.score->unknown1));
      dst.score->score = src.score->score;

      // Player character
      dst.player_character = src.player_character;

      // Unknown 3
      memcpy(dst.unknown3, src.unknown3, sizeof(src.unknown3));
   }

   void CopyState(address::Th9GlobalVer1_5* src, address::Th9GlobalVer1_5* dst)
   {
      CopyBoard(src->board[0], dst->board[0]);
      CopyBoard(src->board[1], dst->board[1]);

      memcpy(dst->unknown1, src->unknown1, sizeof(src->unknown1));

      dst->ex_attack_container = new raw_types::ExAttackContainer();
      memcpy(dst->ex_attack_container->unknown1, src->ex_attack_container->unknown1, sizeof(src->ex_attack_container->unknown1));
      for (int i = 0; i < 256; i++)
      {
         memcpy(dst->ex_attack_container->attacks[i].unknown1, src->ex_attack_container->attacks[i].unknown1, sizeof(src->ex_attack_container->attacks[i].unknown1));
         dst->ex_attack_container->attacks[i].is_2P = src->ex_attack_container->attacks[i].is_2P;
         memcpy(dst->ex_attack_container->attacks[i].unknown6, src->ex_attack_container->attacks[i].unknown6, sizeof(src->ex_attack_container->attacks[i].unknown6));
         dst->ex_attack_container->attacks[i].enabled = src->ex_attack_container->attacks[i].enabled;
         memcpy(dst->ex_attack_container->attacks[i].unknown2, src->ex_attack_container->attacks[i].unknown2, sizeof(src->ex_attack_container->attacks[i].unknown2));
         //dst->ex_attack_container->attacks[i].hitbodies = src->ex_attack_container->attacks[i].hitbodies; ---Pointer
         CopyVector3D(dst->ex_attack_container->attacks[i].position, src->ex_attack_container->attacks[i].position);
         memcpy(dst->ex_attack_container->attacks[i].unknown3, src->ex_attack_container->attacks[i].unknown3, sizeof(src->ex_attack_container->attacks[i].unknown3));

         if (src->ex_attack_container->attacks[i].feature)
         {
            dst->ex_attack_container->attacks[i].feature = new raw_types::ExFeature();
            dst->ex_attack_container->attacks[i].feature->is_local = src->ex_attack_container->attacks[i].feature->is_local;
            CopyVector3D(dst->ex_attack_container->attacks[i].feature->velocity, src->ex_attack_container->attacks[i].feature->velocity);
            memcpy(dst->ex_attack_container->attacks[i].feature->unknown1, src->ex_attack_container->attacks[i].feature->unknown1, sizeof(src->ex_attack_container->attacks[i].feature->unknown1));
            CopyVector3D(dst->ex_attack_container->attacks[i].feature->velocity_mystia, src->ex_attack_container->attacks[i].feature->velocity_mystia);
            memcpy(dst->ex_attack_container->attacks[i].feature->unknown2, src->ex_attack_container->attacks[i].feature->unknown2, sizeof(src->ex_attack_container->attacks[i].feature->unknown2));
            dst->ex_attack_container->attacks[i].feature->yuuka_or_aya_info = src->ex_attack_container->attacks[i].feature->yuuka_or_aya_info;
            dst->ex_attack_container->attacks[i].feature->reisen_radius_or_aya_offset = src->ex_attack_container->attacks[i].feature->reisen_radius_or_aya_offset;
            memcpy(dst->ex_attack_container->attacks[i].feature->unknown3, src->ex_attack_container->attacks[i].feature->unknown3, sizeof(src->ex_attack_container->attacks[i].feature->unknown3));
            for (int j = 0; j < 4; j++)
            {
               CopyVector3D(dst->ex_attack_container->attacks[i].feature->knives[j].position, src->ex_attack_container->attacks[i].feature->knives[j].position);
               memcpy(dst->ex_attack_container->attacks[i].feature->knives[j].unknown1, src->ex_attack_container->attacks[i].feature->knives[j].unknown1, sizeof(src->ex_attack_container->attacks[i].feature->knives[j].unknown1));
            }
         }

         memcpy(dst->ex_attack_container->attacks[i].unknown4, src->ex_attack_container->attacks[i].unknown4, sizeof(src->ex_attack_container->attacks[i].unknown4));
         dst->ex_attack_container->attacks[i].update_func_address = src->ex_attack_container->attacks[i].update_func_address;
         memcpy(dst->ex_attack_container->attacks[i].unknown5, src->ex_attack_container->attacks[i].unknown5, sizeof(src->ex_attack_container->attacks[i].unknown5));
      }

      memcpy(dst->unknown2, src->unknown2, sizeof(src->unknown2));
      dst->round = src->round;
      memcpy(dst->unknown3, src->unknown3, sizeof(src->unknown3));
      dst->round_win[0] = src->round_win[0];
      dst->round_win[1] = src->round_win[1];
      memcpy(dst->unknown4, src->unknown4, sizeof(src->unknown4));
      dst->difficulty = src->difficulty;
      memcpy(dst->unknown5, src->unknown5, sizeof(src->unknown5));
      dst->play_status = src->play_status;
      memcpy(dst->unknown6, src->unknown6, sizeof(src->unknown6));
      // key_states: not saved
      memcpy(dst->unknown7, src->unknown7, sizeof(src->unknown7));
      // hwnd: not saved
      memcpy(dst->unknown8, src->unknown8, sizeof(src->unknown8));
      // d3d8 and d3d8_device: not saved
      memcpy(dst->unknown9, src->unknown9, sizeof(src->unknown9));
      dst->charge_types[0] = src->charge_types[0];
      dst->charge_types[1] = src->charge_types[1];
      memcpy(dst->unknown10, src->unknown10, sizeof(src->unknown10));
      // net_info: not saved
   }
}