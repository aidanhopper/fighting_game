#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include "camera.h"
#include "object.h"
#include "pair.h"

void DrawObject(Camera *camera, Object *object);
void DrawBackgroundColor(SDL_Renderer *renderer, SDL_Color *color);

#endif
