#include "object.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_rect.h>

void UpdatePlayerObject(PlayerObject *player,
                        CollisionObject *collisionObject[],
                        int collisionObjectCount) {

  if (player->jump == 1) {
    player->acceleration.y = -1;
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
      CollisionObject *collisionObj = collisionObject[i];
      if (SDL_HasIntersectionF(&collisionObj->object.frect,
                               &player->object.frect)) {
        if (player->velocity.x < 0)
          player->object.frect.x =
              collisionObj->object.frect.x + collisionObj->object.frect.w;
        else
          player->object.frect.x =
              collisionObj->object.frect.x - player->object.frect.w;
      }
    }
  }

  // update position and check for collision on y
  if (player->velocity.y != 0) {
    player->object.frect.y += player->velocity.y;
    for (int i = 0; i < collisionObjectCount; i++) {
      CollisionObject *collisionObj = collisionObject[i];
      if (SDL_HasIntersectionF(&collisionObj->object.frect,
                               &player->object.frect)) {
        if (player->velocity.y < 0 && !collisionObj->jump_through)
          player->object.frect.y =
              collisionObj->object.frect.y + collisionObj->object.frect.h;
        else {
          player->object.frect.y =
              collisionObj->object.frect.y - player->object.frect.h;
          player->jump = 0;
        }
      }
    }
  }

  if (player->velocity.x > 0)
    player->facing_left = 0;
  else if (player->velocity.x < 0)
    player->facing_left = 1;

  // reset acceleration
  player->acceleration.x = 0;
  player->acceleration.y += 0.07;
}
