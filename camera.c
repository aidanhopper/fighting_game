#include "camera.h"
#include "object.h"
#include <SDL2/SDL_opengl.h>
#include <math.h>

void UpdateCamera(Camera *camera, PlayerObject *player) {

  // setting a guard
  if (camera->scale < 0.01)
    camera->scale = 0.01;
  else if (camera->scale >= camera->boundary.x/camera->frect.w) {
    camera->scale = camera->boundary.x/camera->frect.w;
  }

  Pair difference = {player->object.frect.x - camera->frect.x,
                     player->object.frect.y - camera->frect.y};

  camera->frect.x += difference.x / camera->followDelay;
  camera->frect.y += difference.y / camera->followDelay;

  double scaleFactor = camera->scale;

  double halfWidth = scaleFactor * camera->frect.w / 2.;
  double halfHeight = scaleFactor * camera->frect.h / 2.;

  double left = -halfWidth + camera->frect.x / 2. + player->object.frect.x / 2 +
                player->object.frect.w / 2;
  double right = halfWidth + camera->frect.x / 2. + player->object.frect.x / 2 +
                 player->object.frect.w / 2;
  double bottom = halfHeight + camera->frect.y / 2. +
                  player->object.frect.y / 2 + player->object.frect.h / 2;
  double top = -halfHeight + camera->frect.y / 2. + player->object.frect.y / 2 +
               player->object.frect.h / 2;

  if (left < 0) {
    left = 0;
    right = scaleFactor * camera->frect.w;
  }

  else if (right > camera->boundary.x) {
    right = camera->frect.w;
    left = camera->frect.w - camera->frect.w * scaleFactor;
  }

  //printf("left: %lf right: %lf bottom: %lf top: %lf\n", left, right, bottom, top);
  //printf("scale: %lf\n", camera->scale);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(left, right, bottom, top, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
