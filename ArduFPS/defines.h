#ifndef DEFINES_H
#define DEFINES_H

#ifdef ESP8266
#include "../../../Shared.h"
#define sound (*arduboy_tones)
#else
#include <Arduboy2.h>
#include <ArduboyTones.h>
extern ArduboyTones sound;
#endif

#define VERSION "0.1"

#define TARGET_FRAMERATE 30

#define MAP_WIDTH 32
#define MAP_HEIGHT 32


#endif