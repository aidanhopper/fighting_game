#include "draw.h"
#include <SDL2/SDL_opengl.h>

void DrawObject(Camera *camera, Object *object) {
  SDL_FRect copy = object->frect;
  copy.x -= camera->frect.x;
  copy.x *= camera->zoom;
  copy.y -= camera->frect.y;
  copy.y *= camera->zoom;
  copy.w *= camera->zoom;
  copy.h *= camera->zoom;
  glColor3f(object->color.r / 255.0, object->color.g / 255.0,
            object->color.b / 255.0);
  glBegin(GL_QUADS);
  glVertex2f(copy.x, copy.y);
  glVertex2f(copy.x + copy.w, copy.y);
  glVertex2f(copy.x + copy.w,
             copy.y + copy.h);
  glVertex2f(copy.x, copy.y + copy.h);
  glEnd();
}

void DrawBackgroundColor(SDL_Color *color) {
  glClearColor(color->r / 255.0, color->g / 255.0,
            color->b / 255.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}
