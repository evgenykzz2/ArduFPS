#include "defines.h"
#include "Game.h"

#ifdef ESP8266
#define sprites arduboy_sprites
#else
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites;
#endif


#ifdef ESP8266
void Game_ArduFPS_setup()
#else
void setup()
#endif
{  
  #ifdef ESP8266
  arduboy_tones = new ArduboyTones(arduboy.audio.enabled);
  #endif

  arduboy.begin();
  arduboy.setFrameRate(TARGET_FRAMERATE);
}

#ifdef ESP8266
void Game_ArduFPS_loop()
#else
void loop()
#endif
{
  if (!arduboy.nextFrame())
    return;
  arduboy.display(true);
}
