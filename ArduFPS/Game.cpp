#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Render.h"
#include "Tables.h"
#include "Textures.h"
#include "defines.h"

namespace ArduFPS
{

void Game::Init()
{
}

void Game::StartLevel()
{
  Player::x = 1*256 + 128;
  Player::y = 1*256 + 128;
  Player::angle = 0;

  Map::BuildMap(8, 8);
}

void Game::Control()
{
  uint8_t buttons = arduboy.buttonsState();
  if (buttons & RIGHT_BUTTON)
  {
    Player::angle -= 5;
    if (Player::angle < 0)
      Player::angle += 360;
  }
  if (buttons & LEFT_BUTTON)
  {
    Player::angle += 5;
    if (Player::angle >= 360)
      Player::angle -= 360;
  }

  int16_t dx, dy;
  if (buttons & UP_BUTTON)
  {
    dx = ((int16_t)pgm_read_word(g_cos + Player::angle) / MOVEMENT_SPEED_DIVIDER);
    dy = ((int16_t)pgm_read_word(g_sin + Player::angle) / MOVEMENT_SPEED_DIVIDER);
  } else if (buttons & DOWN_BUTTON)
  {
    dx = -((int16_t)pgm_read_word(g_cos + Player::angle) / MOVEMENT_SPEED_DIVIDER);
    dy = -((int16_t)pgm_read_word(g_sin + Player::angle) / MOVEMENT_SPEED_DIVIDER);
  } else
  {
    dx = 0;
    dy = 0;
  }

  //Calculate wall collisions
  if (dx != 0 || dy != 0)
  {
    if (dx > 0)
    {
      int8_t iy = (Player::y) >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        int8_t ix = (Player::x + dx + MOVEMENT_COLISION) >> 8;
        if (ix >= 0 && ix < MAP_WIDTH)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != 0)
            dx = (int16_t)ix*256 - Player::x - MOVEMENT_COLISION;
        }
      }
    } else if (dx < 0)
    {
      int8_t iy = (Player::y) >> 8;
      if (iy >= 0 && iy < MAP_HEIGHT)
      {
        int8_t ix = (Player::x + dx - MOVEMENT_COLISION) >> 8;
        if (ix >= 0 && ix < MAP_WIDTH)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != 0)
            dx = (int16_t)(ix+1)*256 - Player::x + MOVEMENT_COLISION;
        }
      }
    }

    if (dy > 0)
    {
      int8_t ix = (Player::x) >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        int8_t iy = (Player::y + dy + MOVEMENT_COLISION) >> 8;
        if (iy >= 0 && iy < MAP_HEIGHT)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != 0)
            dy = (int16_t)iy*256 - Player::y - MOVEMENT_COLISION;
        }
      }
    } else if (dy < 0)
    {
      int8_t ix = (Player::x) >> 8;
      if (ix >= 0 && ix < MAP_WIDTH)
      {
        int8_t iy = (Player::y + dy - MOVEMENT_COLISION) >> 8;
        if (iy >= 0 && iy < MAP_HEIGHT)
        {
          if (Map::m_cell[(int16_t)iy*MAP_WIDTH+(int16_t)ix] != 0)
            dy = (int16_t)(iy+1)*256 - Player::y + MOVEMENT_COLISION;
        }
      }
    }

    Player::x += dx;
    Player::y += dy;
  }
}

void Game::Draw()
{
  Render::Prepare();
  Render::RenderMap();

  Sprites::drawPlusMask(64-12, 64-18, g_weapon_sprites, 0);
}

}
