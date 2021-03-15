#ifndef LEVELS_H
#define LEVELS_H
#include <stdint.h>

namespace ArduFPS
{

#define LEVEL_8x8     0
#define LEVEL_8x16    1
#define LEVEL_16x16   2
#define LEVEL_16x32   3

class Level
{
public:
  static const uint8_t s_tile_set[];
  static const uint8_t s_level[];

  static void Load(uint8_t level);
};

}

#endif
