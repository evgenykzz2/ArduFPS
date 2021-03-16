#include "Levels.h"
#include "Map.h"
#include <avr/pgmspace.h>

#define ID_START  16
#define ID_FINISH 17

namespace ArduFPS
{

void Level::Load(uint8_t level)
{
  uint16_t pos = 0;
  uint16_t tileset_offset = 0;
  uint8_t desc = pgm_read_byte(s_level + pos); pos ++;
  uint8_t size = desc & 0x03;

  if (size == LEVEL_8x8)
  {
    Map::m_map_width = 8;
    Map::m_map_height = 8;
  } else if (size == LEVEL_8x16)
  {
    Map::m_map_width = 8;
    Map::m_map_height = 16;
  } else if (size == LEVEL_16x16)
  {
    Map::m_map_width = 16;
    Map::m_map_height = 16;
  } else if (size == LEVEL_16x32)
  {
    Map::m_map_width = 16;
    Map::m_map_height = 32;
  }

  Map::m_cell_start_x = 1;
  Map::m_cell_start_y = 1;
  
  Map::m_cell_finish_x = Map::m_map_width-1;
  Map::m_cell_finish_y = Map::m_map_height-1;

  uint16_t bit_buffer = pgm_read_byte(s_level + pos) | ( pgm_read_byte(s_level + pos + 1) << 8); pos += 2;
  uint8_t bit_pos = 0;
  for (uint8_t y = 0; y < Map::m_map_height; ++y)
  {
    for (uint8_t x = 0; x < Map::m_map_width; ++x)
    {
      uint16_t index = (uint16_t)x + (uint16_t)y*MAP_WIDTH;
      uint8_t b = (bit_buffer >> bit_pos) & 1;
      bit_pos ++;
      if (b == 0)
      {
        uint8_t t = (bit_buffer >> bit_pos) & 1;
        bit_pos ++;
        if (t == 0)
          Map::m_cell[index] = pgm_read_byte(s_tile_set + tileset_offset + 0);
        else
          Map::m_cell[index] = pgm_read_byte(s_tile_set + tileset_offset + 1);
      } else
      {
        uint8_t t = (bit_buffer >> bit_pos) & 31;
        bit_pos += 5;
        Map::m_cell[index] = pgm_read_byte(s_tile_set + tileset_offset + t);
        if (t == ID_START)
        {
          Map::m_cell[index] = CELL_EMPTY;
          Map::m_cell_start_x = x;
          Map::m_cell_start_y = y;
        } else if (t == ID_FINISH)
        {
          Map::m_cell[index] = CELL_EMPTY;
          Map::m_cell_finish_x = x;
          Map::m_cell_finish_y = y;
        }
      }
      if (bit_pos >= 8)
      {
        bit_buffer >>= 8;
        bit_pos -= 8;
        bit_buffer |= (pgm_read_byte(s_level + pos) << 8);
        pos += 1;
      }
    }
  }

  /*for (uint8_t y = 0; y < Map::m_map_height; ++y)
  {
    for (uint8_t x = 0; x < Map::m_map_width; ++x)
    {
      uint16_t index = (uint16_t)x + (uint16_t)y*MAP_WIDTH;
      if ( Map::m_cell[index] < CELL_DOOR_LIMIT && Map::m_cell[index] != CELL_EMPTY )
      {
        //Check Door Orientation
        if (Map::m_cell[index+1] != CELL_EMPTY)
          Map::m_cell[index] |= CELL_FLAG_HORIZONTAL;
      }
    }
  }*/
  Map::DoorReset();
}

}
