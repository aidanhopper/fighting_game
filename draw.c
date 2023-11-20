#include "draw.h"
#include <SDL2/SDL_opengl.h>

void DrawPlayerObject(Camera *camera, PlayerObject *player) {
  SDL_FRect copy = player->object.frect;
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, player->object.textureid);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);

  if (player->facing_left) { 
    glVertex2f(copy.x, copy.y);
    glTexCoord2f(-1., 0.);

    glVertex2f(copy.x + copy.w, copy.y);
    glTexCoord2f(-1., 1.);

    glVertex2f(copy.x + copy.w, copy.y + copy.h);
    glTexCoord2f(0., 1.);

    glVertex2f(copy.x, copy.y + copy.h);
    glTexCoord2f(0., 0.);
  }
  else {
    glVertex2f(copy.x, copy.y);
    glTexCoord2f(1., 0.);

    glVertex2f(copy.x + copy.w, copy.y);
    glTexCoord2f(1., 1.);

    glVertex2f(copy.x + copy.w, copy.y + copy.h);
    glTexCoord2f(0., 1.);

    glVertex2f(copy.x, copy.y + copy.h);
    glTexCoord2f(0., 0.);
  }

  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void DrawColorObject(Camera *camera, Object *object) {
  SDL_FRect copy = object->frect;
  //glEnable(GL_COLOR_MATERIAL);
  //glEnable(GL_BLEND);
  //glEnable(GL_LIGHTING);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glColor3f(object->color.r / 255.0, object->color.g / 255.0,
  //          object->color.b / 255.0);
  glBegin(GL_QUADS);
  glVertex2f(copy.x, copy.y);
  glVertex2f(copy.x + copy.w, copy.y);
  glVertex2f(copy.x + copy.w, copy.y + copy.h);
  glVertex2f(copy.x, copy.y + copy.h);
  //glDisable(GL_COLOR_MATERIAL);
  //glDisable(GL_BLEND);
  //glDisable(GL_LIGHTING);
  glEnd();
}

void DrawBackgroundColor(SDL_Color *color) {
  glClearColor(color->r / 255.0, color->g / 255.0, color->b / 255.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}
