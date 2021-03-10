#include "Render.h"
#include "Player.h"

namespace ArduFPS
{

int16_t Render::z_buffer[128];

void RenderCell(int8_t x, int8_t y, uint8_t flags)
{
  x *= 256;
  y *= 256;

  //Left side
  if (Player::x < x && (uint8_t)(flags & CELL_SIDE_MASK_LEFT))
    RenderWall(x, y+256, x, y, cell, CELL_SIDE_LEFT);

  //Right side
  if (Player::x > x && (uint8_t)(flags & CELL_SIDE_MASK_RIGHT))
    RenderWall(x+256, y, x+256, y+256, cell, CELL_SIDE_RIGHT);

  //Bottom side
  if (Player::y < y && (uint8_t)(flags & CELL_SIDE_MASK_BOTTOM))
    RenderWall(x, y, x+256, y, cell, CELL_SIDE_BOTTOM);

  //Top side
  if (Player::y > y && (uint8_t)(flags & CELL_SIDE_MASK_TOP))
    RenderWall(x+256, y+256, x, y+256, cell, CELL_SIDE_TOP); 
}

void RenderCells()
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

  //Render from front to back
  //cell_in_view = 0;
  //g_line_done = 0;
  //side_in_view = 0;
  if (abs(angle_cos) < abs(angle_sin))
  {
    for (int8_t ry=y_start; ry != y_end; ry += y_add)
    {
      int16_t cell_index = ry*MAP_WIDTH + x_start;
      for (int8_t rx=x_start; rx != x_end; rx += x_add)
      {
        uint8_t flags = Map::m_cell_flags[cell_index];
        if (flags == 0)
        {
          cell_index++;
          continue;
        }
        RenderCell(rx, ry, flags);
        //if (g_line_done == RENDER_WIDTH)
        //  break;
        cell_index++;
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
        //if (g_map_cell[rx+ry*32] == 0 || g_map_cell_flags[rx+ry*32] == 0)
        //  continue;
        //RenderCell(rx, ry, g_map_cell[rx+ry*32], g_map_cell_flags[rx+ry*32]);
        //if (g_line_done == RENDER_WIDTH)
        //  break;
      }
      //if (g_line_done == RENDER_WIDTH)
      //  break;
    }
  }
}



}
