#ifndef GAME_H
#define GAME_H
#include <stdint.h>

namespace ArduFPS
{

class Game
{
public:
  static void Init();
  static void StartLevel();
  static void Control();
  static void Draw();
};

}

#endif
