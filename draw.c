#include "draw.h"
#include <SDL2/SDL_render.h>

void DrawObject(Camera *camera, Object *object) {
  SDL_SetRenderDrawColor(camera->renderer, object->color.r, object->color.g, object->color.b, object->color.a);
  SDL_FRect copy = object->frect;
  copy.x -= camera->frect.x;
  copy.x *= camera->zoom;
  copy.y -= camera->frect.y;
  copy.y *= camera->zoom;
  copy.w *= camera->zoom;
  copy.h *= camera->zoom;
  SDL_RenderFillRectF(camera->renderer, &copy);
}

void DrawBackgroundColor(SDL_Renderer *renderer, SDL_Color *color) {
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
  SDL_RenderClear(renderer);
}
