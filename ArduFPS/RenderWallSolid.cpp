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

void Render::RenderWallSegmentTextured(int16_t x1, int16_t w1, int16_t u1, int16_t x2, int16_t w2, int16_t u2)
{
  if (x1 < 0)
  {
    u1 += ((int32_t)(0 - x1) * (int32_t)(u2 - u1)) / (x2 - x1);
    w1 += ((int32_t)(0 - x1) * (int32_t)(w2 - w1)) / (x2 - x1);
    x1 = 0;
  }

  int16_t dx = x2 - x1;
  int16_t werror = dx / 2;
  int16_t w = w1;
  int16_t dw;
  int16_t wstep;
  if (w1 < w2)
  {
    dw = w2 - w1;
    wstep = 1;
  } else
  {
    dw = w1 - w2;
    wstep = -1;
  }

  //texture
  int16_t uerror = werror;
  int16_t u = u1;
  int16_t du;
  int16_t ustep;
  if (u1 < u2)
  {
    du = u2 - u1;
    ustep = 1;
  } else
  {
    du = u1 - u2;
    ustep = -1;
  }

  for (int16_t x = x1; x < WIDTH; x++)
  {
    if (x >= x2)
      break;
    if (Render::z_buffer[x] == 0)
    {
      /////////////////////////////    NEW line, no need to draw black pixels
      Render::z_buffer[x] = w > 255 ? 255 : (uint8_t) w;
      int16_t y0 = 32-w/2;  //y0 will be filled
      int16_t y1 = 33+w/2;  //y1 will not be filled
      int16_t v0 = 0;
      int16_t v1 = TEXTURE_SIZE;

      int16_t dy = y1 - y0;
      int16_t verror = dy / 2;
      int16_t v = v0;
      int16_t dv = v1-v0;
      int16_t ur = (u / w) & (TEXTURE_SIZE-1);

      uint16_t txt_pixel = pgm_read_word(g_texture + ur + s_render_texture*TEXTURE_SIZE*TEXTURE_SIZE/16);
      if (y0 < 0)
      {
        verror -= dv*(-y0);
        int16_t d = (-verror + (dy-1)) / dy;
        v += d;
        txt_pixel >>= d;
        verror += d*dy;
        y0 = 0;
      }
      {
        if (y1 > HEIGHT)
          y1 = HEIGHT;

        uint8_t y_begin = 8-(y0 & 7);
        if (y0+y_begin > y1)
          y_begin = y1-y0;
        if (y_begin != 0)
        {
          uint8_t bt = 1 << ((uint8_t)y0 & 7);
          uint16_t row_offset = ((uint8_t)y0 & 0xF8) * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          for (uint8_t y = 0; y < y_begin; ++y)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          y0 += y_begin;
          arduboy.sBuffer[row_offset] = data;
        }

        uint8_t full_blocks = (y1-y0)/8;
        for (uint8_t y = 0; y < full_blocks; ++y)
        {
          uint16_t row_offset = (uint16_t)y0 * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          uint8_t bt = 1;
          for (uint8_t yi = 0; yi < 8; ++yi)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          arduboy.sBuffer[row_offset] = data;
          y0 += 8;
        }

        uint8_t y_end = y1 & 7;
        if (y0 != y1 && y_end != 0)
        {
          uint8_t bt = 1;
          uint16_t row_offset = y0 * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          for (uint8_t y = 0; y < y_end; ++y)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          y0 += y_end;
          arduboy.sBuffer[row_offset] = data;
        }   
      }
      /*y0 ++;
      for (int16_t y = y0; y < HEIGHT; ++y)
      {
        if ( (uint8_t)((x ^ y) & 0x01) == 1 )
        {
          uint8_t bt = 1 << ((uint8_t)y & 7);
          uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
          arduboy.sBuffer[row_offset] |= bt;
        }
      }*/
    } else if (w > Render::z_buffer[x])
    {
      Render::z_buffer[x] = w > 255 ? 255 : (uint8_t) w;
      int16_t y0 = 32-w/2;  //y0 will be filled
      int16_t y1 = 33+w/2;  //y1 will not be filled
      int16_t v0 = 0;
      int16_t v1 = TEXTURE_SIZE;

      int16_t dy = y1 - y0;
      int16_t verror = dy / 2;
      int16_t v = v0;
      int16_t dv = v1-v0;
      int16_t ur = (u / w) & (TEXTURE_SIZE-1);

      uint16_t txt_pixel = pgm_read_word(g_texture + ur + s_render_texture*TEXTURE_SIZE*TEXTURE_SIZE/16);
      if (y0 < 0)
      {
        verror -= dv*(-y0);
        int16_t d = (-verror + (dy-1)) / dy;
        v += d;
        txt_pixel >>= d;
        verror += d*dy;
        y0 = 0;
      }
      {
        if (y1 > HEIGHT)
          y1 = HEIGHT;

        uint8_t y_begin = 8-(y0 & 7);
        if (y0+y_begin > y1)
          y_begin = y1-y0;
        if (y_begin != 0)
        {
          uint8_t bt = 1 << ((uint8_t)y0 & 7);
          uint16_t row_offset = ((uint8_t)y0 & 0xF8) * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          for (uint8_t y = 0; y < y_begin; ++y)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            else
              data &= ~bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          y0 += y_begin;
          arduboy.sBuffer[row_offset] = data;
        }

        uint8_t full_blocks = (y1-y0)/8;
        for (uint8_t y = 0; y < full_blocks; ++y)
        {
          uint16_t row_offset = (uint16_t)y0 * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          uint8_t bt = 1;
          for (uint8_t yi = 0; yi < 8; ++yi)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            else
              data &= ~bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          arduboy.sBuffer[row_offset] = data;
          y0 += 8;
        }

        uint8_t y_end = y1 & 7;
        if (y0 != y1 && y_end != 0)
        {
          uint8_t bt = 1;
          uint16_t row_offset = y0 * WIDTH / 8 + (uint16_t)x;
          uint8_t data = arduboy.sBuffer[row_offset];
          for (uint8_t y = 0; y < y_end; ++y)
          {
            if ( (uint8_t)(txt_pixel & 1) != 0 )
              data |= bt;
            else
              data &= ~bt;
            bt <<= 1;
            verror -= dv;
            while (verror < 0)
            {
              v++;
              verror += dy;
              txt_pixel >>= 1;
            }
          }
          y0 += y_end;
          arduboy.sBuffer[row_offset] = data;
        }   
      }
    }

    werror -= dw;
    while (werror < 0)
    {
      w += wstep;
      werror += dx;
    }
    uerror -= du;
    while(uerror < 0)
    {
      u+=ustep;
      uerror += dx;
    }
  }
}

#pragma GCC pop_options

}
