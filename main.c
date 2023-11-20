#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define ASSERT(_e, ...)                                                        \
  if (!(_e)) {                                                                 \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(1);                                                                   \
  }

#define SIGN(n) ({                                                             \
    __typeof__(n) _n = (n);                                                    \
    (__typeof__(n))(_n < 0 ? -1 : (_n > 0 ? 1 : 0));                           \
  })                                                                           \

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef ssize_t isize;

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216
#define MAP_SIZE 24 

#define PI 3.1415

static u8 MAPDATA[MAP_SIZE * MAP_SIZE] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

typedef struct v2_s {
  f32 x, y;
} v2;

typedef struct v2i_s {
  i32 x, y;
} v2i;

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  SDL_Surface *textureBuffer[10];
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
  bool quit;
  v2 pos, dir, plane;
} state;

void verline(int x, int y0, int y1, u32 color) {
  for (int y = y0; y <= y1; y++) {
    state.pixels[(y * SCREEN_WIDTH) + x] = color;
  }
}

void rotate(f32 rot) {
  const v2 d = state.dir, p = state.plane;
  state.dir.x = d.x * cos(rot) - d.y * sin(rot);
  state.dir.y = d.x * sin(rot) + d.y * cos(rot);
  state.plane.x = p.x * cos(rot) - p.y * sin(rot);
  state.plane.y = p.x * sin(rot) + p.y * cos(rot);
}

void render() {
  for (int x = 0; x < SCREEN_WIDTH; x++) {

    //calculate ray position and direction
    double cameraX = 2 * x / (double) SCREEN_WIDTH - 1; //x-coordinate in camera space
    v2 dir = {
      state.dir.x + state.plane.x * cameraX,
      state.dir.y + state.plane.y * cameraX
    };

    //which box of the map we're in
    v2i ipos = {
      (int) state.pos.x,
      (int) state.pos.y
    };

    v2 pos = state.pos;

    //length of ray from current position to next x or y-side

    //length of ray from one x or y-side to next x or y-side
    v2 deltaDist = {
      (dir.x == 0) ? 1e30 : fabs(1.0 / dir.x),
      (dir.y == 0) ? 1e30 : fabs(1.0 / dir.y)
    };

    double perpWallDist;

    //what direction to step in x or y-direction (either +1 or -1)
    v2i step = {
      (int) SIGN(dir.x), (int) SIGN(dir.y)
    };

    int hit = 0; //was there a wall hit?
    int side; //was a NS or a EW wall hit?


    //calculate step and initial sideDist
    v2 sideDist = {
      deltaDist.x * (dir.x < 0 ? (pos.x - ipos.x) : (ipos.x + 1 - pos.x)),
      deltaDist.y * (dir.y < 0 ? (pos.y - ipos.y) : (ipos.y + 1 - pos.y))
    };

    //perform DDA
    while (hit == 0)
    {
      //jump to next map square, either in x-direction, or in y-direction
      if (sideDist.x < sideDist.y)
      {
        sideDist.x += deltaDist.x;
        ipos.x += step.x;
        side = 0;
      }
      else
      {
        sideDist.y += deltaDist.y;
        ipos.y += step.y;
        side = 1;
      }
      //Check if ray has hit a wall
      if (MAPDATA[ipos.y * MAP_SIZE + ipos.x] > 0) hit = 1;
    } 

    //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    if(side == 0) perpWallDist = (sideDist.x - deltaDist.x);
    else          perpWallDist = (sideDist.y - deltaDist.y);

    int lineHeight = (int) (SCREEN_WIDTH / perpWallDist);
    int y0 = -lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (y0 < 0)
      y0 = 0;
    int y1 = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (y1 >= SCREEN_HEIGHT) y1 = SCREEN_HEIGHT - 1;

    u32 color = 0xFFFFFF;
    if (side == 1) {
      const u32
        br = ((color & 0xFF00FF) * 0xC0) >> 8,
           g  = ((color & 0x00FF00) * 0xC0) >> 8;

      color = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
    }

    verline(x, 0, y0, 0xFF202020);
    verline(x, y0, y1, color);
    verline(x, y1, SCREEN_HEIGHT - 1, 0x87CEEB);

  }
}

int main() {
  // do setup
  ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to intialize: %s\n",
         SDL_GetError());
  state.window = SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                                  SDL_WINDOWPOS_CENTERED_DISPLAY(0), 1280, 720,
                                  SDL_WINDOW_ALLOW_HIGHDPI);
  ASSERT(state.window, "SDL failed to create window: %s\n", SDL_GetError());
  state.renderer =
      SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
  ASSERT(state.renderer, "SDL failed to create renderer: %s\n", SDL_GetError());
  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
  ASSERT(state.texture, "SDL failed to create texture: %s\n", SDL_GetError());

  state.textureBuffer[0] = IMG_Load("./texture.png");
  
  // game
  state.pos = (v2){4, 4};
  state.dir.x = -1;
  state.dir.y = 0;
  state.plane.x = 0; state.plane.y = 2;
  while (!state.quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        state.quit = true;
        break;
      }
    }

    const u8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_LEFT]) {
      rotate(0.1);
    }

    if (keystate[SDL_SCANCODE_RIGHT]) {
      rotate(-0.1);
    }

    if (keystate[SDL_SCANCODE_UP]) {
      state.pos.x += state.dir.x * 0.1;
      state.pos.y += state.dir.y * 0.1;
    }

    if (keystate[SDL_SCANCODE_DOWN]) {
      state.pos.x -= state.dir.x * 0.1;
      state.pos.y -= state.dir.y * 0.1;
    }

    memset(state.pixels, 0, sizeof(state.pixels));

    render();

    SDL_UpdateTexture(state.texture, NULL, state.pixels,
                      SCREEN_WIDTH * sizeof(u32));

    SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL, 0.0, NULL,
                     SDL_FLIP_VERTICAL);
    SDL_RenderPresent(state.renderer);
  }

  // clean up
  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  return 0;
}
