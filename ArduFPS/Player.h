#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

namespace ArduFPS
{

class Player
{
  static bool CollideCell(uint8_t cell_x, uint8_t cell_y, uint8_t id);
public:
  static int16_t x;
  static int16_t y;
  static int16_t angle;
  
  static void TeleportToMapStart();
  static void Control(uint8_t buttons);
};

}

#endif
