#include "Render.h"
#include "Player.h"
#include "Map.h"
#include "Tables.h"
#include "Textures.h"
#include "defines.h"

namespace ArduFPS
{
#pragma GCC push_options
#pragma GCC optimize ("O3")

void Render::RenderSprite(int16_t x, int16_t y, uint8_t scale, uint8_t sprite_index)
{
  int16_t x_local, y_local;
  World2Local(x, y, &x_local, &y_local);

  //Don't render too near, or back walls
  if (y_local < FRONT_CLIP)
    return;

  //Apply perspective projection
  int16_t x_screen = (int16_t)(((int32_t)x_local * NEAR_PLANE) / (int32_t)y_local) + (WIDTH/2);
  int16_t sprite_size = (int16_t)(((int32_t)scale * NEAR_PLANE) / (int32_t)y_local);

  int16_t x0 = x_screen - sprite_size;
  int16_t x1 = x_screen + sprite_size;
  
  //Out of screen
  if ( x0 >= WIDTH || x1 < 0)
    return;

  uint16_t texture_offset = (uint16_t)sprite_index*(16*2);
  extern const uint16_t g_objects[];

  int16_t wall_height = (int16_t)(((int32_t)WALL_HEIGHT * NEAR_PLANE) / (int32_t)y_local);

  int16_t u0 = 0;
  int16_t u1 = SPRITE_SIZE;
  if (x0 < 0)
  {
    u0 += ((int32_t)(0 - x0) * (int32_t)(u1 - u0)) / (x1 - x0);
    x0 = 0;
  }

  int16_t dx = x1 - x0;
  int16_t uerror = dx / 2;
  int16_t u = u0;
  int16_t du = u1-u0;
  if (x1 > WIDTH)
    x1 = WIDTH;

  int16_t y0 = 32 + wall_height/2 - sprite_size*2;
  int16_t y1 = y0 + sprite_size*2;
  int16_t v0 = 0;
  int16_t v1 = TEXTURE_SIZE;

  int16_t dy = y1 - y0;
  int16_t verror = dy / 2;
  int16_t v = v0;
  int16_t dv = v1-v0;

  uint8_t skip_pixels = 0;
  if (y0 < 0)
  {
    verror -= dv*(-y0);
    int16_t d = (-verror + (dy-1)) / dy;
    v += d;
    skip_pixels = (uint8_t)d;
    verror += d*dy;
    y0 = 0;
  }

  if (y1 > HEIGHT)
    y1 = HEIGHT;
      
  for (int16_t x = x0; x < x1; ++x)
  {
    if (wall_height > Render::z_buffer[x])
    {
      uint16_t data = pgm_read_word(g_objects + texture_offset + u*2 + 0);
      uint16_t mask = pgm_read_word(g_objects + texture_offset + u*2 + 1);
      data >>= skip_pixels;
      mask >>= skip_pixels;

      if (mask != 0)
      {
        int16_t verror_current = verror;
    
        for (int16_t y = y0; y < y1; ++y)
        {
          if ((uint8_t)(mask & 1) != 0)
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            if ((uint8_t)(data & 1) != 0)
              arduboy.sBuffer[row_offset] |= bt;
            else
              arduboy.sBuffer[row_offset] &= ~bt;
          }   
    
          verror_current -= dv;
          while (verror_current < 0)
          {
            verror_current += dy;
            data >>= 1;
            mask >>= 1;
          }
        }
        //Draw vertical line
        //arduboy.drawFastVLine(x, y0, sprite_size*2);
      }
    }
    
    uerror -= du;
    while (uerror < 0)
    {
      u ++;
      uerror += dx;
    }
  }
}

#pragma GCC pop_options

}
