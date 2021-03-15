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
  static int16_t s_world2local_cos;
  static int16_t s_world2local_sin;
  static uint8_t s_render_cell;
  static uint8_t s_render_texture;
  static uint8_t s_render_side;

  static void RenderWallSegmentTextured(int16_t x1, int16_t w1, int16_t u1, int16_t x2, int16_t w2, int16_t u2);
  static void World2Local(int16_t x, int16_t y, int16_t* x_out, int16_t* y_out);
  
  static void RenderSprite(int16_t x, int16_t y, uint8_t scale, uint8_t sprite_index);
  static void Prepare();
  static void RenderMap();
};

}

#endif
