#include "Levels.h"

namespace ArduFPS
{

#define LEVEL_8x8     0
#define LEVEL_8x16    1
#define LEVEL_16x16   2

static const uint8_t s_tileset[] PROGMEM =
{
  0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x09, 0x0A, 0x0B, 0x0F, 0x12, 0x13, 0x14, 0x10, 0x11
};

static const s_level[] PROGMEM =
{
};

void Level::Load(uint8_t level)
{
}

}
