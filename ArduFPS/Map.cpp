#include "Map.h"
#include "Player.h"
#include "Tables.h"
#include "Enemy.h"

namespace ArduFPS
{

static const uint8_t s_cell_animation_mask[] PROGMEM =
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //Doors
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t s_cell_animation_transform[] PROGMEM =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Doors
  0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x17, 0x18, 0x15, 0x00, 0x00, 0x1C, 0x1D, 0x1E, 0x1B, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t Map::m_cell[MAP_WIDTH*MAP_HEIGHT];
uint16_t Map::m_enemy_bit_flag[MAP_HEIGHT];

uint8_t Map::m_map_width;
uint8_t Map::m_map_height;

uint8_t Map::m_cell_start_x;
uint8_t Map::m_cell_start_y;
  
uint8_t Map::m_cell_finish_x;
uint8_t Map::m_cell_finish_y;
  
uint8_t Map::m_current_door_cell_x;
uint8_t Map::m_current_door_cell_y;
int16_t Map::m_current_door_progress;
uint8_t Map::m_current_door_open_counter;
uint8_t Map::m_current_door_direction;

void Map::BuildMap(uint8_t w, uint8_t h)
{
  //Clean all
  for (uint16_t i = 0; i < MAP_WIDTH*MAP_HEIGHT; ++i)
  {
    m_cell[i] = CELL_EMPTY;
    //m_cell_flags[i] = 0;
  }

  //Set border walls
  for (uint8_t y = 0; y < h; ++y)
  {
    for (uint8_t x = 0; x < h; ++x)
    {
      if (x == 0 || x+1 == w || y == 0 || y+1 == h)
        m_cell[(uint16_t)x + (uint16_t)y*MAP_WIDTH] = 0;
    }
  }

  m_map_width = w;
  m_map_height = h;

  DoorReset();

  m_cell[2 + 3*MAP_WIDTH] = 0;
  m_cell[2 + 4*MAP_WIDTH] = 5;// | CELL_FLAG_DOOR;
  m_cell[2 + 5*MAP_WIDTH] = 0x0B;

  m_cell[3 + 3*MAP_WIDTH] = 5;//2 | CELL_FLAG_DOOR | CELL_FLAG_HORIZONTAL;
  m_cell[4 + 3*MAP_WIDTH] = 0;
  
  AnalizeMap();
}

void Map::AnalizeMap()
{
  //Analize level map
  /*for (uint8_t y = 0; y < m_map_height; ++y)
  {
    for (uint8_t x = 0; x < m_map_width; ++x)
    {
      uint16_t index = x+y*MAP_WIDTH;
      m_cell_flags[index] = 0;
      if (m_cell[index] == CELL_EMPTY)
        continue;

      if (x != 0 && (m_cell[index-1] == CELL_EMPTY || (uint8_t)(m_cell[index-1] & CELL_FLAG_DOOR) != 0))
        m_cell_flags[index] |= CELL_SIDE_MASK_LEFT;
      if (x+1 != m_map_width && (m_cell[index+1] == CELL_EMPTY || (uint8_t)(m_cell[index+1] & CELL_FLAG_DOOR) != 0))
        m_cell_flags[index] |= CELL_SIDE_MASK_RIGHT;

      if (y != 0 && (m_cell[index-MAP_WIDTH] == CELL_EMPTY || (uint8_t)(m_cell[index-MAP_WIDTH] & CELL_FLAG_DOOR) != 0))
        m_cell_flags[index] |= CELL_SIDE_MASK_BOTTOM;
      if (y+1 != m_map_height && (m_cell[index+MAP_WIDTH] == CELL_EMPTY || (uint8_t)(m_cell[index+MAP_WIDTH] & CELL_FLAG_DOOR) != 0))
        m_cell_flags[index] |= CELL_SIDE_MASK_TOP;
    }
  }*/
}

void Map::Control()
{
  //Door control
  if (m_current_door_cell_x != 0)
  {
    //Door is open, wait X frames
    if (m_current_door_open_counter != 0)
    {
      //if ((uint8_t)(arduboy.frameCount & 3) == 0)
      if ((Player::x >> 8) != m_current_door_cell_x || (Player::y >> 8) != m_current_door_cell_y)
      {
        m_current_door_open_counter --;
        if (m_current_door_open_counter == 0)
        {
          m_current_door_direction = 1;
        }
      }
    }

    //Door openning
    if (m_current_door_direction == 0 && m_current_door_progress < 256)
    {
      m_current_door_progress = (m_current_door_progress * DOOR_OPEN_SPEED_MULTIPLAYER + 280*(16-DOOR_OPEN_SPEED_MULTIPLAYER)) / 16;
      //m_current_door_progress += 8;
      if (m_current_door_progress >= 256)
      {
        m_current_door_progress = 256;
        m_current_door_open_counter = DOOR_OPEN_DURATION;
      }
    }

    //Door closing
    if (m_current_door_direction == 1 && m_current_door_progress > 0)
    {
      m_current_door_progress = (m_current_door_progress * DOOR_OPEN_SPEED_MULTIPLAYER + 0*(16-DOOR_OPEN_SPEED_MULTIPLAYER)) / 16;
      //m_current_door_progress -= 8;
      if (m_current_door_progress <= 0)
      {
        m_current_door_progress = 0;
        m_current_door_cell_x = 0;
      }
    }
  }

  //Cell animation
  for (uint8_t y = 0; y < m_map_height; ++y)
  {
    for (uint8_t x = 0; x < m_map_width; ++x)
    {
      uint16_t id = x + y*MAP_WIDTH;
      uint8_t cell = m_cell[id];
      if (cell >= CELL_OBJECT_BASE)
        continue;
      uint8_t mask = pgm_read_byte(s_cell_animation_mask+cell);
      if (mask != 0xFF)
      {
          if ((uint8_t)(arduboy.frameCount & mask) == 0)
            m_cell[id] = pgm_read_byte(s_cell_animation_transform+cell);
      }
    }
  }

  //Activate new enemy
  ActivateNewEnemy();
}

static bool ActivateEnemyAtCell(int8_t x, int8_t y, uint8_t cell)
{
  uint8_t enemy_id = Enemy::FindInactive();
  if (enemy_id == ENEMY_INVALID_ID)
    return false;
  Map::m_cell[x + y*MAP_WIDTH] = CELL_EMPTY;
  
  s_enemy[enemy_id].flags = ENEMY_FLAG_ACTIVE;
  s_enemy[enemy_id].x = x * 256 + 128;
  s_enemy[enemy_id].y = y * 256 + 128;
  s_enemy[enemy_id].type = 0;
  s_enemy[enemy_id].data = 0;
  return true;
}

void Map::ActivateNewEnemy()
{
  if ( Enemy::FindInactive() == ENEMY_INVALID_ID )
    return;
  uint8_t cell_x_min;
  if (Player::x < 0)
    cell_x_min = 0;
  else
    cell_x_min = (uint8_t)(Player::x >> 8);
  
  uint8_t cell_y_min;
  if (Player::y < 0)
    cell_y_min = 0;
  else
    cell_y_min = (uint8_t)(Player::y >> 8);

  int8_t cell_x_max = cell_x_min;
  int8_t cell_y_max = cell_y_min;
  for (int16_t a = -45; a <= 45; a += 45)
  {
    int16_t angle = Player::angle + a;
    if (angle >= 360)
      angle -= 360;
    if (angle <= 0)
      angle += 360;
    int16_t x = (int16_t)pgm_read_word(g_cos + angle)*ENEMY_ACTIVATION_RADIUS + Player::x;
    if (x <= 0)
      cell_x_min = 0;
    else
    {
      int8_t cell = x >> 8;
      if (cell < cell_x_min)
        cell_x_min = cell;
      if (cell > cell_x_max)
        cell_x_max = cell;
    }
    int16_t y = (int16_t)pgm_read_word(g_sin + angle)*ENEMY_ACTIVATION_RADIUS + Player::y;
    if (y <= 0)
      cell_y_min = 0;
    else
    {
      int8_t cell = y >> 8;
      if (cell < cell_y_min)
        cell_y_min = cell;
      if (cell > cell_y_max)
        cell_y_max = cell;
    }
  }
  if (cell_x_max > MAP_WIDTH-1)
    cell_x_max = MAP_WIDTH-1;
  if (cell_y_max > MAP_HEIGHT-1)
    cell_y_max = MAP_HEIGHT-1;

  int8_t x_start;
  int8_t x_end;
  int8_t x_add;
  int16_t angle_cos = (int16_t)pgm_read_word(g_cos + Player::angle);
  if (angle_cos >= 0)
  {
    x_start = cell_x_min;
    x_end = cell_x_max+1;
    x_add = 1;
  } else
  {
    x_start = cell_x_max;
    x_end = cell_x_min-1;
    x_add = -1;
  }

  int8_t y_start;
  int8_t y_end;
  int8_t y_add;
  int16_t angle_sin = (int16_t)pgm_read_word(g_sin + Player::angle);
  if (angle_sin >= 0)
  {
    y_start = cell_y_min;
    y_end = cell_y_max+1;
    y_add = 1;
  } else
  {
    y_start = cell_y_max;
    y_end = cell_y_min-1;
    y_add = -1;
  }

  //Render from front to back
  if (abs(angle_cos) < abs(angle_sin))
  {
    for (int8_t ry=y_start; ry != y_end; ry += y_add)
    {
      for (int8_t rx=x_start; rx != x_end; rx += x_add)
      {
        int16_t cell_index = ry*MAP_WIDTH + rx;
        uint8_t cell_id = Map::m_cell[cell_index];
        if (cell_id == CELL_OBJECT_BASE + 8)
        {
          if (Map::RayTrace(Player::x, Player::y, rx*256+128, ry*256+128))
          {
            if (!ActivateEnemyAtCell(rx, ry, cell_id))
              return;
          }
        }
      }
    }
  } else
  {
    //Draw walls
    for (int8_t rx=x_start; rx != x_end; rx += x_add)
    {
      for (int8_t ry=y_start; ry != y_end; ry += y_add)
      {
        int16_t cell_index = ry*MAP_WIDTH + rx;
        uint8_t cell_id = Map::m_cell[cell_index];
        if (cell_id == CELL_OBJECT_BASE + 8)
        {
          if (Map::RayTrace(Player::x, Player::y, rx*256+128, ry*256+128))
          {
            if (!ActivateEnemyAtCell(rx, ry, cell_id))
              return;
          }
        }
      }
    }
  }
}

void Map::DoorReset()
{
  m_current_door_cell_x = 0;
  m_current_door_cell_y = 0;
  m_current_door_progress = 0;
  m_current_door_open_counter = 0;
  m_current_door_direction = 0;
}

bool Map::RayTrace(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  int8_t x0_cell = x0 >> 8;
  int8_t x1_cell = x1 >> 8;
  int8_t y0_cell = y0 >> 8;
  int8_t y1_cell = y1 >> 8;

  if (x0_cell == x1_cell && y0_cell == y1_cell)
    return true;
  
  int16_t dx = x1-x0;
  int16_t dy = y1-y0;

  //Trace X
  int8_t x_start = x0 >> 8;
  int8_t x_end = x1 >> 8;
  if (dx < 0)
  {
    for (int8_t x = x_start; x > x_end; --x)
    {
      int16_t x_border = (int16_t)(x << 8);
      int16_t y_border = y0 + (int32_t)(x_border - x0) * (int32_t)dy / dx;
      int8_t y = (y_border >> 8);
      if (y < 0 || y >= MAP_HEIGHT)
        continue;
      int16_t index = (x-1) + y*MAP_WIDTH;
      if (m_cell[index] == CELL_EMPTY || m_cell[index] >= CELL_OBJECT_BASE)
        continue;
      if (m_cell[index] < CELL_DOOR_LIMIT &&
          x-1 == m_current_door_cell_x &&
          y == m_current_door_cell_y &&
          m_current_door_progress == 256)
            continue;
      return false;
    }
  } else
  {
    for (int8_t x = x_start+1; x <= x_end; ++x)
    {
      int16_t x_border = (int16_t)(x << 8);
      int16_t y_border = y0 + (int32_t)(x_border - x0) * (int32_t)dy / dx;
      int8_t y = (y_border >> 8);
      if (y < 0 || y >= MAP_HEIGHT)
        continue;
      int16_t index = x + y*MAP_WIDTH;
      if (m_cell[index] == CELL_EMPTY || m_cell[index] >= CELL_OBJECT_BASE)
        continue;
      if (m_cell[index] < CELL_DOOR_LIMIT &&
          x == m_current_door_cell_x &&
          y == m_current_door_cell_y &&
          m_current_door_progress == 256)
            continue;
      return false;
    }
  }

  //Trace Y
  int8_t y_start = y0 >> 8;
  int8_t y_end = y1 >> 8;
  if (dy < 0)
  {
    for (int8_t y = y_start; y > y_end; --y)
    {
      int16_t y_border = (int16_t)(y << 8);
      int16_t x_border = x0 + (int32_t)(y_border - y0) * (int32_t)dx / dy;
      int8_t x = (x_border >> 8);
      if (x < 0 || x >= MAP_WIDTH)
        continue;
      int16_t index = x + (y-1)*MAP_WIDTH;
      if (m_cell[index] == CELL_EMPTY || m_cell[index] >= CELL_OBJECT_BASE)
        continue;
      if (m_cell[index] < CELL_DOOR_LIMIT &&
          x == m_current_door_cell_x &&
          y-1 == m_current_door_cell_y &&
          m_current_door_progress == 256)
            continue;
      return false;
    }
  } else
  {
    for (int8_t y = y_start+1; y <= y_end; ++y)
    {
      int16_t y_border = (int16_t)(y << 8);
      int16_t x_border = x0 + (int32_t)(y_border - y0) * (int32_t)dx / dy;
      int8_t x = (x_border >> 8);
      if (x < 0 || x >= MAP_WIDTH)
        continue;
      int16_t index = x + y*MAP_WIDTH;
      if (m_cell[index] == CELL_EMPTY || m_cell[index] >= CELL_OBJECT_BASE)
        continue;
      if (m_cell[index] < CELL_DOOR_LIMIT &&
          x == m_current_door_cell_x &&
          y == m_current_door_cell_y &&
          m_current_door_progress == 256)
            continue;
      return false;
    }
  }
  return true;
}

}
