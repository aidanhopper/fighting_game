#include "camera.h"
#include "draw.h"
#include "object.h"
#include "utils.h"
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>

const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 720;

double camerax = 0.0f;
double cameray = 0.0f;

const int FPS = 144;
const int FRAME_DELAY = 1000 / FPS;

GLuint textureID;

void Play(SDL_Window *window);
void Menu(SDL_Window *window);
void loadTexture(const char *path);

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
    // Menu(window);
    break;
  case PLAY:
    Play(window);
    break;
  }

  // clean up
  // CleanUp(renderer, window);
  glDeleteTextures(1, &textureID);
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void Menu(SDL_Window *window);

void Play(SDL_Window *window) {
  SDL_Event event;
  int quit = 0;

  Uint32 frameStart;

  // init player
  loadTexture("./guy.png");
  PlayerObject player;
  player.object.textured = 1;
  player.object.textureid = textureID;
  player.object.frect = (SDL_FRect){100, 100, 16, 48};
  player.velocity = (Pair){0, 0};
  player.maxVelocity = (Pair){3, 6};
  player.jump = 0;
  player.facing_left = 1;

  // init camera
  Camera camera;
  camera.frect = (SDL_FRect){player.object.frect.x, player.object.frect.y,
                             CAMERA_WIDTH, CAMERA_HEIGHT};
  camera.scale = 0.5f;
  camera.followDelay = 8;
  camera.boundary = (Pair){CAMERA_WIDTH*3, CAMERA_HEIGHT*3};

  // create some objects in the world
  int collisionObjectsCount = 0;
  CollisionObject *collisionObjects[100];

  CollisionObject platform1;
  platform1.object.color = (SDL_Color){255, 255, 255, 255};
  platform1.object.frect = (SDL_FRect){0, 300, CAMERA_WIDTH, 10};
  platform1.jump_through = 0;

  CollisionObject platform2;
  platform2.object.color = (SDL_Color){0, 0, 0, 255};
  platform2.object.frect = (SDL_FRect){0, 240, 100, 10};
  platform2.jump_through = 1;

  collisionObjects[collisionObjectsCount++] = &platform1;
  collisionObjects[collisionObjectsCount++] = &platform2;

  SDL_Color bg = {0, 181, 226, 255};


  while (!quit) {
    frameStart = SDL_GetTicks();
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = 1;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    // Handle key presses
    if (state[SDL_SCANCODE_A])
      player.acceleration.x += -.1;
    if (state[SDL_SCANCODE_D])
      player.acceleration.x += .1;
    if (state[SDL_SCANCODE_SPACE] && player.jump == 0)
      player.jump = 1;
    if (state[SDL_SCANCODE_UP])
      camera.scale -= 0.01;
    if (state[SDL_SCANCODE_DOWN])
      camera.scale += 0.01;

    glClear(GL_COLOR_BUFFER_BIT);
    // update here

    UpdatePlayerObject(&player, collisionObjects, collisionObjectsCount);
    UpdateCamera(&camera, &player);

    // draw Here
    DrawBackgroundColor(&bg);
    DrawPlayerObject(&camera, &player);
    DrawColorObject(&camera, &platform1.object);
    DrawColorObject(&camera, &platform2.object);

    SDL_GL_SwapWindow(window);

    // delays to hit target fps
    WaitForNextFrame(frameStart, FRAME_DELAY);
  }
}

void loadTexture(const char *path) {
  SDL_Surface *surface = IMG_Load(path);

  if (!surface) {
    fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
    return;
  }

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SDL_FreeSurface(surface);
}
