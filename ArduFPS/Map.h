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

#define CELL_EMPTY            0x00
#define CELL_DOOR_LIMIT       0x10
#define CELL_OBJECT_BASE      0xE0

#define CELL_OBJECT_PISTOLE     0xE0
#define CELL_OBJECT_SHOTGUN     0xE1
#define CELL_OBJECT_MACHINEGUN  0xE2
#define CELL_OBJECT_ROCKET      0xE3
#define CELL_OBJECT_HEALTH_S    0xE4
#define CELL_OBJECT_HEALTH_L    0xE5
#define CELL_OBJECT_BARREL      0xE6
#define CELL_OBJECT_ARMOR       0xE7

#define CELL_OBJECT_ENEMY_BASE  0xE8

class Map
{
  static void AnalizeMap();
  static void ActivateNewEnemy();
public:
  static uint8_t m_cell[MAP_WIDTH*MAP_HEIGHT];
  static uint16_t m_enemy_bit_flag[MAP_HEIGHT];

  static uint8_t m_map_width;
  static uint8_t m_map_height;

  static uint8_t m_cell_start_x;
  static uint8_t m_cell_start_y;
  
  static uint8_t m_cell_finish_x;
  static uint8_t m_cell_finish_y;
  
  static uint8_t m_current_door_cell_x;
  static uint8_t m_current_door_cell_y;
  static int16_t m_current_door_progress;
  static uint8_t m_current_door_direction;
  static uint8_t m_current_door_open_counter;

  static void DoorReset();
  static void BuildMap(uint8_t w, uint8_t h);
  static void Control();

  static bool RayTrace(int16_t x0, int16_t y0, int16_t x1, int16_t y1); //return true if there is no wall collision
};

}

#endif
