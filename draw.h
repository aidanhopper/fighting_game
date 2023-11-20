#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_opengl.h>
#include "camera.h"
#include "object.h"
#include "pair.h"

void DrawBackgroundColor(SDL_Color *color);
void DrawPlayerObject(Camera *camera, PlayerObject *player);
void DrawColorObject(Camera *camera, Object *object);

#endif
