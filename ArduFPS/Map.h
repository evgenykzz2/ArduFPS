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

class Map
{
  static void AnalizeMap(uint8_t w, uint8_t h);

public:
  static uint8_t m_cell[MAP_WIDTH*MAP_HEIGHT];
  static uint8_t m_cell_flags[MAP_WIDTH*MAP_HEIGHT];
  
  static void BuildMap(uint8_t w, uint8_t h);
};

}

#endif
