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

  static void Prepare();
  static void RenderMap();
};

}

#endif
