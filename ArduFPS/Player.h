#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

namespace ArduFPS
{

class Player
{
public:
  static int16_t x;
  static int16_t y;
  static int16_t angle;

  static void Control(uint8_t buttons);
};

}

#endif
