#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "defines.h"

namespace ArduFPS
{

class Render
{
public:
  static uint8_t z_buffer[WIDTH];

  static void RenderSprite(int16_t x, int16_t y, uint8_t scale, uint8_t sprite_index);
  static void Prepare();
  static void RenderMap();
};

}

#endif
