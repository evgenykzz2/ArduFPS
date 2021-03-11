#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

namespace ArduFPS
{

class Render
{
public:
  static uint8_t z_buffer[128];

  static void Prepare();
  static void RenderMap();
};

}

#endif
