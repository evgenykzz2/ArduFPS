#include "Enemy.h"
#include "Render.h"
#include "Player.h"

namespace ArduFPS
{

Enemy s_enemy[ENEMY_ACTIVE_MAX];

const uint8_t Enemy::s_data[] PROGMEM =
{
//Sprite  Scale  HP   Speed   Damage
  8,      64,    5,   4,      2,      0,0,0,
};

//const uint8_t s_object_data[] PROGMEM =
//{ 
//};

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

void Enemy::Control()
{
  for (uint8_t i = 0; i < ENEMY_ACTIVE_MAX; ++i)
  {
    if ( (uint8_t)(s_enemy[i].flags & ENEMY_FLAG_ACTIVE) == 0 )
      continue;
    uint8_t ofs = s_enemy[i].type * 8;
    uint8_t speed = pgm_read_byte(s_data + ofs + 3);
    
    int16_t dx = Player::x - s_enemy[i].x;
    int16_t dy = Player::y - s_enemy[i].y;
    int16_t len = sqrt((int32_t)dx*(int32_t)dx + (int32_t)dy*(int32_t)dy);

    s_enemy[i].x += (int32_t)dx * (int32_t)speed / len;
    s_enemy[i].y += (int32_t)dy * (int32_t)speed / len;
    //uint8_t ofs = s_enemy[i].type * 8;
    //uint8_t pgm_read_byte(s_data + ofs + 0);
  }
}

void Enemy::Draw(uint8_t index)
{
    uint8_t ofs = s_enemy[index].type * 8;
    uint8_t sprite_base = pgm_read_byte(s_data + ofs + 0);
    uint8_t sprite_scale = pgm_read_byte(s_data + ofs + 1);
    Render::RenderSprite(s_enemy[index].x, s_enemy[index].y, sprite_scale, sprite_base);
}

}
