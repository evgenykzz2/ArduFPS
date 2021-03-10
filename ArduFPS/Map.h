#ifndef MAP_H
#define MAP_H

namespace ArduFPS
{
  
#define CELL_SIDE_MASK_LEFT   1
#define CELL_SIDE_MASK_RIGHT  2
#define CELL_SIDE_MASK_BOTTOM 4
#define CELL_SIDE_MASK_TOP    8

class Map
{
public:
  static uint8_t m_cell[MAP_WIDTH*MAP_HEIGHT];
  static uint8_t m_cell_flags[MAP_WIDTH*MAP_HEIGHT];
};

}

#endif