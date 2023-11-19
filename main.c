#include "camera.h"
#include "draw.h"
#include "object.h"
#include "utils.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_opengl.h>

const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 720;

const int FPS = 144;
const int FRAME_DELAY = 1000 / FPS;

void Menu(SDL_Renderer *renderer);
void Play(SDL_Renderer *renderer);

typedef enum {
  MENU = 0,
  PLAY = 1,
} State;

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  // create window
  SDL_Window *window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, CAMERA_WIDTH,
                                        CAMERA_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    fprintf(stderr, "Window created failed: %s\n", SDL_GetError());
    return 1;
  }

  // create renderer
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    fprintf(stderr, "Render created failed: %s\n", SDL_GetError());
    return 1;
  }

  State state = MENU;
  switch(state) {
    case MENU:
      Menu(renderer);
      break;
    case PLAY:
      Play(renderer);
      break;
  }

  // clean up
  CleanUp(renderer, window);

  return 0;
}

void Menu(SDL_Renderer *renderer) {
  SDL_Event event;
  int quit = 0;

  Uint32 frameStart;
  while (!quit) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = 1;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    WaitForNextFrame(frameStart, FRAME_DELAY);
  }
}

void Play(SDL_Renderer *renderer) {
  SDL_Event event;
  int quit = 0;

  Uint32 frameStart;

  Camera camera;
  camera.renderer = renderer;
  camera.frect = (SDL_FRect) {10, 10, CAMERA_WIDTH, CAMERA_HEIGHT};
  camera.zoom = 5;
  camera.followDelay = 50;
  camera.boundary = (Pair){CAMERA_WIDTH, CAMERA_HEIGHT};

  PlayerObject player;
  player.object.color = (SDL_Color) {0, 0, 0, 255};
  player.object.frect = (SDL_FRect) {700, 250, 10, 10};
  player.velocity = (Pair){0, 0};
  player.maxVelocity = (Pair){1, 1};

  int collisionObjectsCount = 0;
  Object *collisionObjects[100];

  Object platform;
  platform.color = (SDL_Color) {0 ,0 ,0 ,255};
  platform.frect = (SDL_FRect) {0, 300, CAMERA_WIDTH, 30};

  collisionObjects[collisionObjectsCount++] = &platform;

  SDL_Color bg = {0, 181, 226, 255};
  while (!quit) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = 1;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Handle key presses
    if (state[SDL_SCANCODE_W]) {
      player.acceleration.y += -.1;
    }
    if (state[SDL_SCANCODE_S]) {
      player.acceleration.y += .1;
    }
    if (state[SDL_SCANCODE_A]) {
      player.acceleration.x += -.1;
    }
    if (state[SDL_SCANCODE_D]) {
      player.acceleration.x += .1;
    }

    // update here
    UpdatePlayerObject(&player, collisionObjects, collisionObjectsCount);
    UpdateCamera(&camera, &player.object);    

    // draw Here
    DrawBackgroundColor(renderer, &bg);
    DrawObject(&camera, &platform);
    DrawObject(&camera, &player.object);

    // present everything drawn
    SDL_RenderPresent(renderer);

    // delays to hit target fps
    WaitForNextFrame(frameStart, FRAME_DELAY);
  }
}

