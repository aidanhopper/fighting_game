#include "camera.h"

void UpdateCamera(Camera *camera, Object *object) {
  printf("%lf\n", camera->frect.x);
  double targetx = object->frect.x - camera->frect.w / 2 / camera->zoom +
                   object->frect.w/2;
  double targety = object->frect.y - camera->frect.h / 2 / camera->zoom +
                   object->frect.h/2;
  double padding = 0;
  double distancex = targetx - camera->frect.x;
  if (fabs(distancex) > padding)
    camera->frect.x += distancex / camera->followDelay;
  // check the boundary
  if (camera->frect.x < 0)
    camera->frect.x = 0.1;
  if (camera->frect.x + camera->frect.w / camera->zoom > camera->boundary.x)
    camera->frect.x = camera->boundary.x - camera->frect.w / camera->zoom;

  double distancey = targety - camera->frect.y;
  if (fabs(distancey) > padding)
    camera->frect.y += distancey / camera->followDelay;
  if (camera->frect.y < 0)
    camera->frect.y = 0.1;
  if (camera->frect.y + camera->frect.h / camera->zoom > camera->boundary.y)
    camera->frect.y = camera->boundary.y - camera->frect.h / camera->zoom;

}
