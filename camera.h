#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <math.h>

typedef struct {
  SDL_Renderer *renderer;
  SDL_FRect frect;
  Pair boundary;
  double zoom;
  double followDelay;
} Camera;

void UpdateCamera(Camera *camera, Object *object);

#endif
