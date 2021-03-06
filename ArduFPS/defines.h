#ifndef DEFINES_H
#define DEFINES_H

#ifdef ESP8266
#include "../../../Shared.h"
#define sound (*arduboy_tones)
#else
#include <Arduboy2.h>
#include <ArduboyTones.h>
extern Arduboy2 arduboy;
extern ArduboyTones sound;
#endif

#define VERSION "0.1"
#define MAP_WIDTH 16
#define MAP_HEIGHT 32
#define ENEMY_ACTIVE_MAX 4
#define ENEMY_ACTIVATION_RADIUS 6

#define TARGET_FRAMERATE 30
#define RENDER_RADIUS 10
#define MOVEMENT_COLISION 48
#define MOVEMENT_SPEED_DIVIDER 16
#define MOVEMENT_ROTATION_SPEED 5

#define RENDER_WIDTH   (WIDTH-16)

#define FRONT_CLIP 16
#define WALL_HEIGHT 180
#define NEAR_PLANE 64

#define DOOR_OPEN_DURATION 90
#define DOOR_OPEN_SPEED_MULTIPLAYER 14

#endif
