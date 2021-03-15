#ifndef MAP_H
#define MAP_H
#include <stdint.h>
#include "defines.h"

namespace ArduFPS
{
  
#define CELL_SIDE_MASK_LEFT   0x10
#define CELL_SIDE_MASK_RIGHT  0x20
#define CELL_SIDE_MASK_BOTTOM 0x40
#define CELL_SIDE_MASK_TOP    0x80

#define CELL_EMPTY_FLAG       0x80
#define CELL_EMPTY            0xFF
#define CELL_MASK_TEXTURE     0x3F
#define CELL_FLAG_DOOR        0x80
#define CELL_FLAG_HORIZONTAL  0x40

class Map
{
  static void AnalizeMap();
public:
  static uint8_t m_cell[MAP_WIDTH*MAP_HEIGHT];
  //static uint8_t m_cell_flags[MAP_WIDTH*MAP_HEIGHT];

  static uint8_t m_map_width;
  static uint8_t m_map_height;
  
  static uint8_t m_current_door_cell_x;
  static uint8_t m_current_door_cell_y;
  static uint8_t m_current_door_progress;
  
  static void BuildMap(uint8_t w, uint8_t h);
  static void Control();
};

}

#endif
