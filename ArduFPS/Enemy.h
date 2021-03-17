#ifndef ENEMY_H
#define ENEMY_H

#include "defines.h"
#include <stdint.h>

namespace ArduFPS
{

#define ENEMY_INVALID_ID      0xFF

#define ENEMY_FLAG_ACTIVE     0x80
#define ENEMY_FLAG_STATE_MASK 0x03
#define ENEMY_STATE_STAY      0x00
#define ENEMY_STATE_WALK      0x01
#define ENEMY_STATE_ATTACK    0x02
#define ENEMY_STATE_DIE       0x03

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
  static void Move(uint8_t index, int16_t dx, int16_t dy);
};

extern Enemy s_enemy[ENEMY_ACTIVE_MAX];
//extern const uint8_t s_object_data[];

}

#endif
