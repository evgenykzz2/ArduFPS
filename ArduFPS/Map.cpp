#include "Map.h"

namespace ArduFPS
{

static const uint8_t s_cell_animation_mask[] PROGMEM =
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x01, 0x01, 0x01, 0x01, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t s_cell_animation_transform[] PROGMEM =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0x08, 0x05, 0x00, 0x00, 0x0C, 0x0D, 0x0E, 0x0B, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t Map::m_cell[MAP_WIDTH*MAP_HEIGHT];
//uint8_t Map::m_cell_flags[MAP_WIDTH*MAP_HEIGHT];

uint8_t Map::m_map_width;
uint8_t Map::m_map_height;
  
uint8_t Map::m_current_door_cell_x;
uint8_t Map::m_current_door_cell_y;
uint8_t Map::m_current_door_progress;

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

  m_current_door_cell_x = 0;
  m_current_door_cell_y = 0;
  m_current_door_progress = 0;

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
  for (uint8_t y = 0; y < m_map_height; ++y)
  {
    uint16_t id = y*MAP_WIDTH;
    for (uint8_t x = 0; x < m_map_width; ++x)
    {
      uint8_t cell = m_cell[id];
      if (m_cell[id] != CELL_EMPTY)
      {
        uint8_t mask = pgm_read_byte(s_cell_animation_mask+cell);
        if (mask != 0xFF)
        {
          if ((uint8_t)(arduboy.frameCount & mask) == 0)
            m_cell[id] = pgm_read_byte(s_cell_animation_transform+cell);
        }
      }
      ++id;
    }
  }
}

}
