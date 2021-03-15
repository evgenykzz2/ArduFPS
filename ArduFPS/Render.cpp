#include "Render.h"
#include "Player.h"
#include "Map.h"
#include "Tables.h"
#include "Textures.h"
#include "defines.h"

namespace ArduFPS
{
uint8_t Render::z_buffer[WIDTH];

int16_t Render::s_world2local_cos;
int16_t Render::s_world2local_sin;
uint8_t Render::s_render_cell;
uint8_t Render::s_render_texture;
uint8_t Render::s_render_side;

void Render::World2Local(int16_t x, int16_t y, int16_t* x_out, int16_t* y_out)
{
  int16_t x_local = x - Player::x;
  int16_t y_local = y - Player::y;

  //Hi accuracy
  *x_out = (int16_t)(( (uint32_t)s_world2local_cos * (uint32_t)x_local - (uint32_t)s_world2local_sin * (uint32_t)y_local) >> 8);
  *y_out = (int16_t)(( (uint32_t)s_world2local_sin * (uint32_t)x_local + (uint32_t)s_world2local_cos * (uint32_t)y_local) >> 8);

  //Midium accuracy, works bad
  //*x_out = (int16_t)((s_world2local_cos * x_local) >> 8) - (int16_t)((s_world2local_sin * y_local) >> 8);
  //*y_out = (int16_t)((s_world2local_sin * x_local) >> 8) + (int16_t)((s_world2local_cos * y_local) >> 8);
}

void RenderWallSegment(int16_t x1, int16_t w1, int16_t x2, int16_t w2)
{
  if (x1 < 0)
  {
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

  for (int16_t x = x1; x < WIDTH; x++)
  {
    if (x >= x2)
      break;
    if (w > Render::z_buffer[x])
    {
      Render::z_buffer[x] = w > 255 ? 255 : w;
      /*if (w > 40)
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          uint8_t bt = 1 << ((uint8_t)y & 7);
          uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
          arduboy.sBuffer[row_offset] |= bt;
        }
      } else if (w > 20)
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y | (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      } else if (w > 10)
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y ^ (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      } else
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y & (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      }*/
      
      if ( (uint8_t)(s_render_side & CELL_SIDE_MASK_LEFT) != 0 )
      {
        //arduboy.drawFastVLine(x, 32 - w/2, w);
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          uint8_t bt = 1 << ((uint8_t)y & 7);
          uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
          arduboy.sBuffer[row_offset] |= bt;
        }
      } else if ( (uint8_t)(s_render_side & CELL_SIDE_MASK_RIGHT) != 0 )
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y | (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      } else if ( (uint8_t)(s_render_side & CELL_SIDE_MASK_TOP) != 0 )
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y & (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      } else
      {
        int16_t y0 = 32-w/2;
        int16_t y1 = 32+w/2;
        if (y0 < 0)
          y0 = 0;
        for (int16_t y = y0; y <= y1; ++y)
        {
          if (y >= HEIGHT)
            break;
          if ( (uint8_t)(((uint8_t)y ^ (uint8_t)x) & 1) != 0 )
          {
            uint8_t bt = 1 << ((uint8_t)y & 7);
            uint16_t row_offset = ((uint8_t)y & 0xF8) * WIDTH / 8 + (uint16_t)x;
            arduboy.sBuffer[row_offset] |= bt;
          }
        }
      }
    }
    werror -= dw;
    while (werror < 0)
    {
      w += wstep;
      werror += dx;
    }
  }
}

void RenderWall(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  int16_t x0_view, y0_view;
  World2Local(x0, y0, &x0_view, &y0_view);

  int16_t x1_view, y1_view;
  World2Local(x1, y1, &x1_view, &y1_view);

  //Cut wall. Is it good?
  if (x1_view < 0 && -2 * y1_view > x1_view)
    return;
  if (x0_view > 0 &&  2 * y0_view < x0_view)
    return;

  //Don't render too near, or back walls
  if ((y0_view < FRONT_CLIP) && (y1_view < FRONT_CLIP))
    return;

  //textures
  int16_t r_u0 = 0;
  int16_t r_u1 = TEXTURE_SIZE;
  if (y0_view < FRONT_CLIP)
  {
    r_u0 += (FRONT_CLIP - y0_view) * (TEXTURE_SIZE) / (y1_view - y0_view);
    x0_view += (FRONT_CLIP - y0_view) * (x1_view - x0_view) / (y1_view - y0_view);
    y0_view = FRONT_CLIP;
  } else if (y1_view < FRONT_CLIP)
  {
    r_u1 += (FRONT_CLIP - y1_view) * (-TEXTURE_SIZE) / (y0_view - y1_view);
    x1_view += (FRONT_CLIP - y1_view) * (x0_view - x1_view) / (y0_view - y1_view);
    y1_view = FRONT_CLIP;
  }

  //Apply perspective projection
  int16_t vx0 = (int16_t)(((int32_t)x0_view * NEAR_PLANE) / (int32_t)y0_view);
  int16_t vx1 = (int16_t)(((int32_t)x1_view * NEAR_PLANE) / (int32_t)y1_view);

  //Transform to screen pixels
  int16_t sx1 = (int16_t)((WIDTH/2) + vx0);
  int16_t sx2 = (int16_t)((WIDTH/2) + vx1);

  //Out of screen
  if (sx1 >= sx2 || sx2 < 0 || sx1 >= WIDTH)
    return;

  //Calculate wall height
  //w1 = g_wall_height[y0_view];
  //w2 = g_wall_height[y1_view];
  int16_t w1 = (int16_t)((WALL_HEIGHT * NEAR_PLANE) / y0_view);
  int16_t w2 = (int16_t)((WALL_HEIGHT * NEAR_PLANE) / y1_view);

  if (r_u0 != 0)  //optimization
    r_u0 = (int16_t)(((int32_t)r_u0 * WALL_HEIGHT * NEAR_PLANE) / y0_view);
  r_u1 = (int16_t)(((int32_t)r_u1 * WALL_HEIGHT * NEAR_PLANE) / y1_view);

  RenderWallSegmentTextured(sx1, w1, r_u0, sx2, w2, r_u1);
  //RenderWallSegment(sx1, w1, sx2, w2);
}

void RenderCell(int8_t x, int8_t y, uint8_t flags)
{
  int16_t xc = (int16_t)x * 256;
  int16_t yc = (int16_t)y * 256;

  if ( (uint8_t)(s_render_cell & CELL_FLAG_DOOR) != 0)
  {
    if ((uint8_t)(s_render_cell & CELL_FLAG_HORIZONTAL) != 0)
    {
      //Bottom side
      if (Player::y < yc && (uint8_t)(flags & CELL_SIDE_MASK_BOTTOM))
      {
        s_render_side = CELL_SIDE_MASK_BOTTOM;
        RenderWall(xc, yc+128, xc+256, yc+128);
      }
      //Top side
      if (Player::y > yc && (uint8_t)(flags & CELL_SIDE_MASK_TOP))
      {
        s_render_side = CELL_SIDE_MASK_TOP;
        RenderWall(xc+256, yc+128, xc, yc+128);
    }
    } else
    {
      //Left side
      if (Player::x < xc && (uint8_t)(flags & CELL_SIDE_MASK_LEFT))
      {
        s_render_side = CELL_SIDE_MASK_LEFT;
        RenderWall(xc+128, yc+256, xc+128, yc);
      }
      //Right side
      if (Player::x > xc && (uint8_t)(flags & CELL_SIDE_MASK_RIGHT))
      {
        s_render_side = CELL_SIDE_MASK_RIGHT;
        RenderWall(xc+128, yc, xc+128, yc+256);
      }
    }
  } else
  {
    //Left side
    if (Player::x < xc && (uint8_t)(flags & CELL_SIDE_MASK_LEFT))
    {
      s_render_side = CELL_SIDE_MASK_LEFT;
      RenderWall(xc, yc+256, xc, yc);
    }
  
    //Right side
    if (Player::x > xc && (uint8_t)(flags & CELL_SIDE_MASK_RIGHT))
    {
      s_render_side = CELL_SIDE_MASK_RIGHT;
      RenderWall(xc+256, yc, xc+256, yc+256);
    }
  
    //Bottom side
    if (Player::y < yc && (uint8_t)(flags & CELL_SIDE_MASK_BOTTOM))
    {
      s_render_side = CELL_SIDE_MASK_BOTTOM;
      RenderWall(xc, yc, xc+256, yc);
    }
  
    //Top side
    if (Player::y > yc && (uint8_t)(flags & CELL_SIDE_MASK_TOP))
    {
      s_render_side = CELL_SIDE_MASK_TOP;
      RenderWall(xc+256, yc+256, xc, yc+256); 
    }
  }
}

void RenderCellFull(int8_t x, int8_t y)
{
  int16_t xc = (int16_t)x * 256;
  int16_t yc = (int16_t)y * 256;

  uint16_t index = x+y*MAP_WIDTH;
  //Left side
  if (Player::x < xc && x > 0 && Map::m_cell[index-1] == CELL_EMPTY)
  {
    s_render_side = CELL_SIDE_MASK_LEFT;
    RenderWall(xc, yc+256, xc, yc);
  }
  
  //Right side
  if (Player::x > xc && x < MAP_WIDTH-1 && Map::m_cell[index+1] == CELL_EMPTY)
  {
    s_render_side = CELL_SIDE_MASK_RIGHT;
    RenderWall(xc+256, yc, xc+256, yc+256);
  }
  
  //Bottom side
  if (Player::y < yc && y > 0 && Map::m_cell[index-MAP_WIDTH] == CELL_EMPTY)
  {
    s_render_side = CELL_SIDE_MASK_BOTTOM;
    RenderWall(xc, yc, xc+256, yc);
  }
  
  //Top side
  if (Player::y > yc && y < MAP_HEIGHT-1 && Map::m_cell[index+MAP_WIDTH] == CELL_EMPTY)
  {
    s_render_side = CELL_SIDE_MASK_TOP;
    RenderWall(xc+256, yc+256, xc, yc+256); 
  }
}

void Render::Prepare()
{
  int16_t angle = (int16_t)90 - Player::angle;
  if (angle >= 360)
    angle -= 360;
  if (angle < 0)
    angle += 360;

  s_world2local_cos = (int16_t)pgm_read_word(g_cos + angle);
  s_world2local_sin = (int16_t)pgm_read_word(g_sin + angle);

  for (uint8_t i = 0; i < WIDTH; ++i)
    z_buffer[i] = 0;
}

void Render::RenderMap()
{
  //Get 'In view' area
  uint8_t cell_x_min;
  if (Player::x < 0)
    cell_x_min = 0;
  else
    cell_x_min = (uint8_t)(Player::x >> 8);
  
  uint8_t cell_y_min;
  if (Player::y < 0)
    cell_y_min = 0;
  else
    cell_y_min = (uint8_t)(Player::y >> 8);

  int8_t cell_x_max = cell_x_min;
  int8_t cell_y_max = cell_y_min;
  for (int16_t a = -45; a <= 45; a += 45)
  {
    int16_t angle = Player::angle + a;
    if (angle >= 360)
      angle -= 360;
    if (angle <= 0)
      angle += 360;
    int16_t x = (int16_t)pgm_read_word(g_cos + angle)*RENDER_RADIUS + Player::x;
    if (x <= 0)
      cell_x_min = 0;
    else
    {
      int8_t cell = x >> 8;
      if (cell < cell_x_min)
        cell_x_min = cell;
      if (cell > cell_x_max)
        cell_x_max = cell;
    }
    int16_t y = (int16_t)pgm_read_word(g_sin + angle)*RENDER_RADIUS + Player::y;
    if (y <= 0)
      cell_y_min = 0;
    else
    {
      int8_t cell = y >> 8;
      if (cell < cell_y_min)
        cell_y_min = cell;
      if (cell > cell_y_max)
        cell_y_max = cell;
    }
  }
  if (cell_x_max > MAP_WIDTH-1)
    cell_x_max = MAP_WIDTH-1;
  if (cell_y_max > MAP_HEIGHT-1)
    cell_y_max = MAP_HEIGHT-1;

  int8_t x_start;
  int8_t x_end;
  int8_t x_add;
  int16_t angle_cos = (int16_t)pgm_read_word(g_cos + Player::angle);
  if (angle_cos >= 0)
  {
    x_start = cell_x_min;
    x_end = cell_x_max+1;
    x_add = 1;
  } else
  {
    x_start = cell_x_max;
    x_end = cell_x_min-1;
    x_add = -1;
  }

  int8_t y_start;
  int8_t y_end;
  int8_t y_add;
  int16_t angle_sin = (int16_t)pgm_read_word(g_sin + Player::angle);
  if (angle_sin >= 0)
  {
    y_start = cell_y_min;
    y_end = cell_y_max+1;
    y_add = 1;
  } else
  {
    y_start = cell_y_max;
    y_end = cell_y_min-1;
    y_add = -1;
  }

  //Full map
  /*for (int8_t ry=0; ry != MAP_HEIGHT; ry ++)
  {
    for (int8_t rx=0; rx != MAP_WIDTH; rx ++)
    {
      int16_t cell_index = (int16_t)ry*MAP_WIDTH + (int16_t)rx;
      uint8_t flags = Map::m_cell_flags[cell_index];
      if (flags == 0)
        continue;
      s_render_cell = Map::m_cell[cell_index];
      RenderCell(rx, ry, flags);
    }
  }*/

  //Render from front to back
  //cell_in_view = 0;
  //g_line_done = 0;
  //side_in_view = 0;
  if (abs(angle_cos) < abs(angle_sin))
  {
    for (int8_t ry=y_start; ry != y_end; ry += y_add)
    {
      for (int8_t rx=x_start; rx != x_end; rx += x_add)
      {
        int16_t cell_index = ry*MAP_WIDTH + rx;
        //uint8_t flags = Map::m_cell_flags[cell_index];
        //if (flags == 0)
        //  continue;
        s_render_cell = Map::m_cell[cell_index];
        if (s_render_cell == CELL_EMPTY)
          continue;
        s_render_texture = s_render_cell & CELL_MASK_TEXTURE;
        RenderCellFull(rx, ry);
        //if (g_line_done == RENDER_WIDTH)
        //  break;
      }
      //if (g_line_done == RENDER_WIDTH)
      //  break;
    }
  } else
  {
    for (int8_t rx=x_start; rx != x_end; rx += x_add)
    {
      for (int8_t ry=y_start; ry != y_end; ry += y_add)
      {
        int16_t cell_index = ry*MAP_WIDTH + rx;
        //uint8_t flags = Map::m_cell_flags[cell_index];
        //if (flags == 0)
        //  continue;
        s_render_cell = Map::m_cell[cell_index];
        if (s_render_cell == CELL_EMPTY)
          continue;
        s_render_texture = s_render_cell & CELL_MASK_TEXTURE;
        RenderCellFull(rx, ry);
        //if (g_line_done == RENDER_WIDTH)
        //  break;
      }
      //if (g_line_done == RENDER_WIDTH)
      //  break;
    }
  }
}

}
