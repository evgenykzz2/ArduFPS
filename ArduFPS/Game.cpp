#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Render.h"
#include "Levels.h"
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

  //Map::BuildMap(9, 9);
  Level::Load(0);
  Player::TeleportToMapStart();
}

void Game::Control()
{
  uint8_t buttons = arduboy.buttonsState();
  Player::Control(buttons);
  Map::Control();
}

static const uint8_t animation[4] = {8, 9, 8, 10};
void Game::Draw()
{
  Render::Prepare();
  Render::RenderMap();
  
  //Render::RenderSprite(1*256+128, 3*256+128, 48, 0);  //gun
  Render::RenderSprite(1*256+128, 3*256+128, 40, 15);  //armor
  Render::RenderSprite(3*256+128, 3*256+128, 48, 6);  //Health small
  Render::RenderSprite(5*256+128, 3*256+128, 40, 7);  //Health big

  //Sprites::drawPlusMask(64-12, 64-18, g_weapon_sprites, 0);
}

}
