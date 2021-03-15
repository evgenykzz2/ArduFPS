#include "Render.h"
#include "Player.h"
#include "Map.h"
#include "Tables.h"
#include "Textures.h"
#include "defines.h"

namespace ArduFPS
{

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
  for (int16_t x = x0; x < x1; ++x)
  {
    if (wall_height < Render::z_buffer[x])
      continue;

    //Draw vertical line
    arduboy.drawFastVLine(x, y0, sprite_size*2);
    
    uerror -= du;
    while (uerror < 0)
    {
      u ++;
      uerror += dx;
    }
  }
}

}
