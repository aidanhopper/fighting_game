#include "object.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_rect.h>

void UpdatePlayerObject(PlayerObject *player, Object *collisionObject[],
                        int collisionObjectCount) {

  if (player->jump == 1) {
    player->acceleration.y = -3;
    player->jump++;
  } 
    

  // update x velocity
  player->velocity.x += player->acceleration.x;
  if (player->velocity.x > player->maxVelocity.x)
    player->velocity.x = player->maxVelocity.x;
  else if (player->velocity.x < -player->maxVelocity.x)
    player->velocity.x = -player->maxVelocity.x;

  // update y velocity
  player->velocity.y += player->acceleration.y;
  if (player->velocity.y > player->maxVelocity.y)
    player->velocity.y = player->maxVelocity.y;
  else if (player->velocity.y < -player->maxVelocity.y)
    player->velocity.y = -player->maxVelocity.y;

  if (player->acceleration.x == 0)
    player->velocity.x = 0;
  if (player->acceleration.y == 0)
    player->velocity.y = 0;

  // update position and check for collision on x
  if (player->velocity.x != 0) {
    player->object.frect.x += player->velocity.x;
    for (int i = 0; i < collisionObjectCount; i++) {
      Object *collisionObj = collisionObject[i];
      if (SDL_HasIntersectionF(&collisionObj->frect, &player->object.frect)) {
        if (player->velocity.x < 0)
          player->object.frect.x =
              collisionObj->frect.x + collisionObj->frect.w;
        else
          player->object.frect.x =
              collisionObj->frect.x - player->object.frect.w;
      }
    }
  }

  // update position and check for collision on y
  if (player->velocity.y != 0) {
    player->object.frect.y += player->velocity.y;
    for (int i = 0; i < collisionObjectCount; i++) {
      Object *collisionObj = collisionObject[i];
      if (SDL_HasIntersectionF(&collisionObj->frect, &player->object.frect)) {
        if (player->velocity.y < 0)
          player->object.frect.y =
              collisionObj->frect.y + collisionObj->frect.h;
        else {
          player->object.frect.y =
              collisionObj->frect.y - player->object.frect.h;
          player->jump = 0;
        }
      }
    }

  }

  // reset acceleration
  player->acceleration.x = 0;
  player->acceleration.y += 0.1;
}
