#ifndef ENEMY_H
#define ENEMY_H

#include "defines.h"
#include <stdint.h>

namespace ArduFPS
{
#define ENEMY_INVALID_ID      0xFF

#define ENEMY_FLAG_ATTACK     0x10
#define ENEMY_FLAG_ACTIVE     0x80

struct Enemy
{
  int16_t x;
  int16_t y;
  uint8_t hp;
  uint8_t type;
  uint8_t data;
  uint8_t flags;

  static const uint8_t s_data[];

  static void Clean();
  static uint8_t FindInactive();
  static void Control();
  static void Draw(uint8_t index);
};

extern Enemy s_enemy[ENEMY_ACTIVE_MAX];
//extern const uint8_t s_object_data[];

}

#endif
