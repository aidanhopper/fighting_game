#ifndef OBJECT_H
#define OBJECT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include "pair.h"

typedef struct {
  SDL_FRect frect;
  SDL_Color color;
  SDL_Texture *texture;
} Object;

typedef struct {
  Pair velocity;
  Pair acceleration;
  Pair maxVelocity;
  Pair maxAcceleration;
  int jump;
  int maxJumps;
  Object object;
} PlayerObject;

void UpdatePlayerObject(PlayerObject *player, Object *collisionObject[], int collisionObjectCount);
void RenderRect(SDL_FRect *frect);

#endif
