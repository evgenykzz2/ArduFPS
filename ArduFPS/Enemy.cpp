#include "Enemy.h"

namespace ArduFPS
{

Enemy s_enemy[ENEMY_ACTIVE_MAX];

void Enemy::Clean()
{
  for (uint8_t i = 0; i < ENEMY_ACTIVE_MAX; ++i)
    s_enemy[i].flags = 0;
}

uint8_t Enemy::FindInactive()
{
  for (uint8_t i = 0; i < ENEMY_ACTIVE_MAX; ++i)
  {
    if ( (uint8_t)(s_enemy[i].flags & ENEMY_FLAG_ACTIVE) == 0 )
      return i;
  }
  return ENEMY_INVALID_ID;
}

}
