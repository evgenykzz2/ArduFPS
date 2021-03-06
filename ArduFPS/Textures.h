#ifndef TEXTURES_H
#define TEXTURES_H
#include <stdint.h>

#define TEXTURE_SIZE 16
#define SPRITE_SIZE 16

namespace ArduFPS
{
  extern const uint16_t g_texture[];
  extern const uint8_t g_weapon_sprites[];
  extern const uint16_t g_objects[];

  extern const uint8_t g_minifont[];
  extern const uint8_t g_minifont_width[];
  extern const uint8_t g_minifont_offset[];
}

#endif
