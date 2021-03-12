#include "Player.h"
#include "Map.h"
#include "Tables.h"
#include "defines.h"

namespace ArduFPS
{

int16_t Player::x;
int16_t Player::y;
int16_t Player::angle;

void Player::Control(uint8_t buttons)
{
  if (buttons & RIGHT_BUTTON)
  {
    angle -= MOVEMENT_ROTATION_SPEED;
    if (angle < 0)
      angle += 360;
  }
  if (buttons & LEFT_BUTTON)
  {
    angle += MOVEMENT_ROTATION_SPEED;
    if (angle >= 360)
      angle -= 360;
  }

  int16_t dx, dy;
  if (buttons & UP_BUTTON)
  {
    dx = ((int16_t)pgm_read_word(g_cos + angle) / MOVEMENT_SPEED_DIVIDER);
    dy = ((int16_t)pgm_read_word(g_sin + angle) / MOVEMENT_SPEED_DIVIDER);
  } else if (buttons & DOWN_BUTTON)
  {
    dx = -((int16_t)pgm_read_word(g_cos + angle) / MOVEMENT_SPEED_DIVIDER);
    dy = -((int16_t)pgm_read_word(g_sin + angle) / MOVEMENT_SPEED_DIVIDER);
  } else
  {
    dx = 0;
    dy = 0;
  }

  //Calculate wall collisions
  if (dx != 0 || dy != 0)
  {
    if (dx > 0)
    {
      int8_t iy = y >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        int8_t ix = (x + dx + MOVEMENT_COLISION) >> 8;
        if (ix >= 0 && ix < MAP_WIDTH)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != CELL_EMPTY)
            dx = (int16_t)ix*256 - x - MOVEMENT_COLISION;
        }
      }
    } else if (dx < 0)
    {
      int8_t iy = y >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        int8_t ix = (x + dx - MOVEMENT_COLISION) >> 8;
        if (ix >= 0 && ix < MAP_WIDTH)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != CELL_EMPTY)
            dx = (int16_t)(ix+1)*256 - x + MOVEMENT_COLISION;
        }
      }
    }

    if (dy > 0)
    {
      int8_t ix = x >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        int8_t iy = (y + dy + MOVEMENT_COLISION) >> 8;
        if (iy >= 0 && iy < MAP_HEIGHT)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != CELL_EMPTY)
            dy = (int16_t)iy*256 - y - MOVEMENT_COLISION;
        }
      }
    } else if (dy < 0)
    {
      int8_t ix = x >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        int8_t iy = (y + dy - MOVEMENT_COLISION) >> 8;
        if (iy >= 0 && iy < MAP_HEIGHT)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != CELL_EMPTY)
            dy = (int16_t)(iy+1)*256 - y + MOVEMENT_COLISION;
        }
      }
    }

    x += dx;
    y += dy;
  }
}

}
