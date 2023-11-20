#ifndef OBJECT_H
#define OBJECT_H

#include "pair.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

typedef struct {
  SDL_FRect frect;
  SDL_Color color;
  GLuint textureid;
  SDL_bool textured;
} Object;

typedef struct {
  Object object;
  SDL_bool jump_through;
} CollisionObject;

typedef struct {
  Pair velocity;
  Pair acceleration;
  Pair maxVelocity;
  Pair maxAcceleration;
  int jump;
  int maxJumps;
  SDL_bool facing_left;
  Object object;
} PlayerObject;

void UpdatePlayerObject(PlayerObject *player,
                        CollisionObject *collisionObject[],
                        int collisionObjectCount);
void RenderRect(SDL_FRect *frect);

#endif
