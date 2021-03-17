#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Render.h"
#include "Enemy.h"
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
  Enemy::Control();
}

void Game::Draw()
{
  Render::Prepare();
  Render::RenderMap();

  /*for (uint8_t i = 0; i < ENEMY_ACTIVE_MAX; ++i)
  {
    if ( (uint8_t)(s_enemy[i].flags & ENEMY_FLAG_ACTIVE) == 0 )
      continue;
    arduboy.setCursor(0, 32 + i*10);
    arduboy.print(i);
    arduboy.print(F(" "));
    arduboy.print(s_enemy[i].x >> 8);
    arduboy.print(F(" "));
    arduboy.print(s_enemy[i].y >> 8);
  }*/
  //if (Map::RayTrace(Player::x, Player::y, 3*256+128, 6*256+128))
  //  arduboy.drawFastVLine(64, 32, 32);

  //Render::RenderSprite(1*256+128, 3*256+128, 48, 0);  //gun
  //Render::RenderSprite(1*256+128, 3*256+128, 40, 15); //armor
  //Render::RenderSprite(3*256+128, 3*256+128, 48, 6);  //Health small
  //Render::RenderSprite(5*256+128, 3*256+128, 40, 7);  //Health big

  //Sprites::drawPlusMask(64-12, 64-18, g_weapon_sprites, 0);

  static const char s_text[] PROGMEM = "123\0";
  Render::DrawMinifontText(WIDTH-13, 3, s_text);
  Render::DrawMinifontText(WIDTH-13, 9, s_text);
  Render::DrawMinifontText(WIDTH-13, 15, s_text);
  arduboy.drawFastHLine(WIDTH-14, 1, 13, WHITE);
  arduboy.drawFastHLine(WIDTH-14, 21, 13, WHITE);
  arduboy.drawFastVLine(WIDTH-15, 2, 19, WHITE);
  arduboy.drawFastVLine(WIDTH-1, 2, 19, WHITE);
}

}
