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

  Map::BuildMap(9, 9);
}

void Game::Control()
{
  uint8_t buttons = arduboy.buttonsState();
  Player::Control(buttons);
  Map::Control();
}

void Game::Draw()
{
  Render::Prepare();
  Render::RenderMap();
  //Render::RenderSprite(1*256+128, 2*256+128, 32, 0);

  //Sprites::drawPlusMask(64-12, 64-18, g_weapon_sprites, 0);
}

}
