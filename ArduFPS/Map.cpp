#include "Map.h"

namespace ArduFPS
{
  
uint8_t Map::m_cell[MAP_WIDTH*MAP_HEIGHT];
uint8_t Map::m_cell_flags[MAP_WIDTH*MAP_HEIGHT];

void Map::BuildMap(uint8_t w, uint8_t h)
{
  //Clean all
  for (uint16_t i = 0; i < MAP_WIDTH*MAP_HEIGHT; ++i)
  {
    m_cell[i] = 0;
    m_cell_flags[i] = 0;
  }

  //Set border walls
  for (uint8_t y = 0; y < h; ++y)
  {
    for (uint8_t x = 0; x < h; ++x)
    {
      if (x == 0 || x+1 == w || y == 0 || y+1 == h)
        m_cell[(uint16_t)x + (uint16_t)y*MAP_WIDTH] = 1;
    }
  }

  m_cell[2 + 2*MAP_WIDTH] = 1;
  m_cell[2 + 3*MAP_WIDTH] = 1;
  m_cell[2 + 4*MAP_WIDTH] = 4;
  m_cell[2 + 5*MAP_WIDTH] = 1;
  
  AnalizeMap(w, h);
}

void Map::AnalizeMap(uint8_t w, uint8_t h)
{
  //Analize level map
  for (uint8_t y = 0; y < h; ++y)
  {
    for (uint8_t x = 0; x < h; ++x)
    {
      uint16_t index = x+y*MAP_WIDTH;
      m_cell_flags[index] = 0;
      if (m_cell[index] == 0)
        continue;

      if (x != 0 && m_cell[index-1] == 0)
        m_cell_flags[index] |= CELL_SIDE_MASK_LEFT;
      if (x+1 != w && m_cell[index+1] == 0)
        m_cell_flags[index] |= CELL_SIDE_MASK_RIGHT;

      if (y != 0 && m_cell[index-MAP_WIDTH] == 0)
        m_cell_flags[index] |= CELL_SIDE_MASK_BOTTOM;
      if (y+1 != h && m_cell[index+MAP_WIDTH] == 0)
        m_cell_flags[index] |= CELL_SIDE_MASK_TOP;
    }
  }
}

}
