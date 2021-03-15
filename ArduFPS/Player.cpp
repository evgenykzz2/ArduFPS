#include "Player.h"
#include "Map.h"
#include "Tables.h"
#include "defines.h"

namespace ArduFPS
{

int16_t Player::x;
int16_t Player::y;
int16_t Player::angle;

void Player::TeleportToMapStart()
{
  x = Map::m_cell_start_x * 256 + 128;
  y = Map::m_cell_start_y * 256 + 128;
  uint16_t index = Map::m_cell_start_x + Map::m_cell_start_y*MAP_WIDTH;
  if ( (uint8_t)(Map::m_cell[index+1] & CELL_WALKABLE_FLAG) != 0 )
  {
    x -= 64;
    angle = 0;
  } else if ( (uint8_t)(Map::m_cell[index-1] & CELL_WALKABLE_FLAG) != 0 )
  {
    x += 64;
    angle = 180;
  }else if ( (uint8_t)(Map::m_cell[index+MAP_WIDTH] & CELL_WALKABLE_FLAG) != 0 )
  {
    y -= 64;
    angle = 90;
  } else if ( (uint8_t)(Map::m_cell[index-MAP_WIDTH] & CELL_WALKABLE_FLAG) != 0 )
  {
    y += 64;
    angle = 270;
  }
}

bool Player::CollideCell(uint8_t cell_x, uint8_t cell_y, uint8_t id)
{
  if (id == CELL_EMPTY)
    return false;
  else if ( (uint8_t)(id & CELL_FLAG_DOOR) == CELL_FLAG_DOOR )
  {
    if (Map::m_current_door_cell_x == 0 || cell_x != Map::m_current_door_cell_x || cell_y != Map::m_current_door_cell_y)
    {
      Map::m_current_door_cell_x = cell_x;
      Map::m_current_door_cell_y = cell_y;
      Map::m_current_door_progress = 0;
      Map::m_current_door_direction = 0;
      Map::m_current_door_open_counter = 0;
    }
    if (Map::m_current_door_cell_x == cell_x && Map::m_current_door_cell_y == cell_y && Map::m_current_door_progress == 256)
      return false;
    else
      return true;
  } else 
    return true;
}

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
          uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
          if (CollideCell(ix, iy, cell))
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
          uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
          if (CollideCell(ix, iy, cell))
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
          uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
          if (CollideCell(ix, iy, cell))
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
          uint8_t cell = Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix];
          if (CollideCell(ix, iy, cell))
            dy = (int16_t)(iy+1)*256 - y + MOVEMENT_COLISION;
        }
      }
    }

    x += dx;
    y += dy;
  }
}

}
