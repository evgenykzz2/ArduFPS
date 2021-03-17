#include "Enemy.h"
#include "Render.h"
#include "Player.h"
#include "Map.h"

namespace ArduFPS
{

Enemy s_enemy[ENEMY_ACTIVE_MAX];

const uint8_t Enemy::s_data[] PROGMEM =
{
//Sprite  Scale  HP   Speed   Damage   DistMin  DistMax  AttackDelay
  8,      64,    5,   4,      2,       1,       2,       90,
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

void Enemy::Move(uint8_t index, int16_t dx, int16_t dy)
{
  if (dx > 0)
  {
    int8_t iy = s_enemy[index].y >> 8;
    if (iy >= 0 && iy < MAP_HEIGHT)
    {
      int8_t ix = (s_enemy[index].x + dx + MOVEMENT_COLISION) >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
        if (cell != CELL_EMPTY && cell < CELL_OBJECT_BASE)
          dx = (int16_t)ix*256 - s_enemy[index].x - MOVEMENT_COLISION;
      }
    }
  } else if (dx < 0)
  {
    int8_t iy = s_enemy[index].y >> 8;
    if (iy >= 0 && iy < MAP_HEIGHT)
    {
      int8_t ix = (s_enemy[index].x + dx - MOVEMENT_COLISION) >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
        if (cell != CELL_EMPTY && cell < CELL_OBJECT_BASE)
          dx = (int16_t)(ix+1)*256 - s_enemy[index].x + MOVEMENT_COLISION;
      }
    }
  }

  if (dy > 0)
  {
    int8_t ix = s_enemy[index].x >> 8;
    if (ix >= 0 && ix < MAP_WIDTH)
    {
      int8_t iy = (s_enemy[index].y + dy + MOVEMENT_COLISION) >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
        if (cell != CELL_EMPTY && cell < CELL_OBJECT_BASE)
          dy = (int16_t)iy*256 - s_enemy[index].y - MOVEMENT_COLISION;
      }
    }
  } else if (dy < 0)
  {
    int8_t ix = s_enemy[index].x >> 8;
    if (ix >= 0 && ix < MAP_WIDTH)
    {
      int8_t iy = (s_enemy[index].y + dy - MOVEMENT_COLISION) >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
        if (cell != CELL_EMPTY && cell < CELL_OBJECT_BASE)
          dy = (int16_t)(iy+1)*256 - s_enemy[index].y + MOVEMENT_COLISION;
      }
    }
  }

  s_enemy[index].x += dx;
  s_enemy[index].y += dy;
}

void Enemy::Control()
{
  for (uint8_t i = 0; i < ENEMY_ACTIVE_MAX; ++i)
  {
    if ( (uint8_t)(s_enemy[i].flags & ENEMY_FLAG_ACTIVE) == 0 )
      continue;
    uint8_t ofs = s_enemy[i].type * 8;
    uint8_t attack_interval = pgm_read_byte(s_data + ofs + 7);

    uint8_t state = s_enemy[i].flags & ENEMY_FLAG_STATE_MASK;
    s_enemy[i].data ++;
    if (s_enemy[i].data > attack_interval)
    {
      if (s_enemy[i].data > attack_interval+15)
      {
        //Do attack
        s_enemy[i].data = 0;
      } else
      {
        //Attack prepare
        s_enemy[i].flags = (s_enemy[i].flags & (~ENEMY_FLAG_STATE_MASK)) | ENEMY_STATE_ATTACK;
      }
    } else
    {
      uint8_t speed = pgm_read_byte(s_data + ofs + 3);
      uint8_t dist_min = pgm_read_byte(s_data + ofs + 5);
      uint8_t dist_max = pgm_read_byte(s_data + ofs + 6);
      
      int16_t dx = Player::x - s_enemy[i].x;
      int16_t dy = Player::y - s_enemy[i].y;
      int16_t len = sqrt((int32_t)dx*(int32_t)dx + (int32_t)dy*(int32_t)dy);
      if (len < (int16_t)dist_min*256)
      {
        //Go far
        Move(i, -(int32_t)dx * (int32_t)speed / len, -(int32_t)dy * (int32_t)speed / len);
        s_enemy[i].flags = (s_enemy[i].flags & (~ENEMY_FLAG_STATE_MASK)) | ENEMY_STATE_WALK;
      } else if (len > (int16_t)dist_max*256)
      {
        //Go closer
        Move(i, (int32_t)dx * (int32_t)speed / len, (int32_t)dy * (int32_t)speed / len);
        s_enemy[i].flags = (s_enemy[i].flags & (~ENEMY_FLAG_STATE_MASK)) | ENEMY_STATE_WALK;
      } else
        s_enemy[i].flags = (s_enemy[i].flags & (~ENEMY_FLAG_STATE_MASK)) | ENEMY_STATE_STAY;
    }
  }
}

static const uint8_t s_walk_frames[4] PROGMEM = {0, 1, 0, 2};

void Enemy::Draw(uint8_t index)
{
    uint8_t ofs = s_enemy[index].type * 8;
    uint8_t sprite_base = pgm_read_byte(s_data + ofs + 0);
    uint8_t sprite_scale = pgm_read_byte(s_data + ofs + 1);
    uint8_t state = s_enemy[index].flags & ENEMY_FLAG_STATE_MASK;
    if (state == ENEMY_STATE_STAY)
      Render::RenderSprite(s_enemy[index].x, s_enemy[index].y, sprite_scale, sprite_base);
    else if (state == ENEMY_STATE_ATTACK)
      Render::RenderSprite(s_enemy[index].x, s_enemy[index].y, sprite_scale, sprite_base + 3);
    else if (state == ENEMY_STATE_WALK)
      Render::RenderSprite(s_enemy[index].x, s_enemy[index].y, sprite_scale, sprite_base +
              pgm_read_byte(s_walk_frames + ((((uint8_t)arduboy.frameCount + index) >> 2) & 3)));
}

}
