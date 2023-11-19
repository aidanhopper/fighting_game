#include "camera.h"
#include "draw.h"
#include "object.h"
#include "utils.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 720;

const int FPS = 144;
const int FRAME_DELAY = 1000 / FPS;

void Play(SDL_Window *window);
void Menu(SDL_Window *window);
void setOrthographicProjection();
void applyCameraTransform(Camera *camera);

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
  SDL_Window *window = SDL_CreateWindow(
      "SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      CAMERA_WIDTH, CAMERA_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (!window) {
    fprintf(stderr, "Window created failed: %s\n", SDL_GetError());
    return 1;
  }

  // create renderer
  // SDL_Renderer *renderer =
  //    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // create opengl context
  SDL_GLContext glContext = SDL_GL_CreateContext(window);

  if (!glContext) {
    fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  // setup opengl
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

  State state = PLAY;
  switch (state) {
  case MENU:
    Menu(window);
    break;
  case PLAY:
    Play(window);
    break;
  }

  // clean up
  // CleanUp(renderer, window);
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void Menu(SDL_Window *window) {
  SDL_Event event;
  int quit = 0;

  Object platform;
  platform.color = (SDL_Color){100, 0, 0, 255};
  platform.frect = (SDL_FRect){0, 0, 10, 10};

  Camera camera;
  camera.frect = (SDL_FRect){0, 0, CAMERA_WIDTH, CAMERA_HEIGHT};
  camera.zoom = 1;
  camera.followDelay = 50;
  camera.boundary = (Pair){CAMERA_WIDTH, CAMERA_HEIGHT};

  SDL_Color bg = {0, 181, 226, 255};

  Uint32 frameStart;
  setOrthographicProjection();
  while (!quit) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = 1;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    //applyCameraTransform(&camera);

    DrawBackgroundColor(&bg);
    DrawObject(&camera, &platform);

    SDL_GL_SwapWindow(window);

    WaitForNextFrame(frameStart, FRAME_DELAY);
  }
}

void Play(SDL_Window *window) {
  SDL_Event event;
  int quit = 0;

  Uint32 frameStart;

  Camera camera;
  camera.frect = (SDL_FRect){0, 0, CAMERA_WIDTH, CAMERA_HEIGHT};
  camera.zoom = 1;
  camera.followDelay = 40;
  camera.boundary = (Pair){CAMERA_WIDTH, CAMERA_HEIGHT};
  camera.last = (Pair){0, 0};

  PlayerObject player;
  player.object.color = (SDL_Color){0, 0, 0, 255};
  player.object.frect = (SDL_FRect){0, 0, 10, 10};
  player.velocity = (Pair){0, 0};
  player.maxVelocity = (Pair){1, 6};
  player.jump = 0;

  int collisionObjectsCount = 0;
  Object *collisionObjects[100];

  Object platform;
  platform.color = (SDL_Color){0, 0, 0, 255};
  platform.frect = (SDL_FRect){0, 300, CAMERA_WIDTH, 10};

  collisionObjects[collisionObjectsCount++] = &platform;

  SDL_Color bg = {0, 181, 226, 255};

  setOrthographicProjection();
  while (!quit) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = 1;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Handle key presses
    if (state[SDL_SCANCODE_A]) {
      player.acceleration.x += -.1;
    }
    if (state[SDL_SCANCODE_D]) {
      player.acceleration.x += .1;
    }
    if (state[SDL_SCANCODE_W] && player.jump == 0) {
      player.jump = 1;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    // update here
    UpdatePlayerObject(&player, collisionObjects, collisionObjectsCount);
    UpdateCamera(&camera, &player.object);

    // draw Here
    // DrawBackgroundColor(renderer, &bg);
    DrawBackgroundColor(&bg);
    DrawObject(&camera, &player.object);
    DrawObject(&camera, &platform);

    SDL_GL_SwapWindow(window);

    // delays to hit target fps
    WaitForNextFrame(frameStart, FRAME_DELAY);
  }
}

void setOrthographicProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, CAMERA_WIDTH, CAMERA_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void applyCameraTransform(Camera *camera) {
  //glTranslatef(CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2, 0);
  //glScalef(camera->zoom, camera->zoom, 1.0);
  //glTranslatef(0, 0, 0);
}
