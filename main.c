#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
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

#define DOT(v0, v1)                  \
    ({ const v2 _v0 = (v0), _v1 = (v1); (_v0.x * _v1.x) + (_v0.y * _v1.y); })

#define LENGTH(v) ({ const v2 _v = (v); sqrtf(DOT(_v, _v)); })


#define MIN(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define MAX(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })

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
//#define SCREEN_WIDTH 1280
//#define SCREEN_HEIGHT 720
#define MAP_SIZE 24 
#define PI 3.1415
#define FPS 60

typedef struct v4_s {
  f32 x, y, z, w;
} v4;

typedef struct v3_s {
  f32 x, y, z;
} v3;

typedef struct v3i_s {
  i32 x, y, z;
} v3i;

typedef struct v2_s {
  f32 x, y;
} v2;

typedef struct v2i_s {
  i32 x, y;
} v2i;

typedef struct {
  u32 w;
  u32 h;
  u32 *pixels;
} texture;

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
  bool quit;
  texture textures[1];
  float angle;
  v3 pos, dir;
} state;


void rotate(f32 rot) {
  /*
  const v3 d = state.dir, p = state.plane;
  state.dir.x = d.x * cos(rot) - d.y * sin(rot);
  state.dir.y = d.x * sin(rot) + d.y * cos(rot);
  state.plane.x = p.x * cos(rot) - p.y * sin(rot);
  state.plane.y = p.x * sin(rot) + p.y * cos(rot);
  */
}

void drawpixel(int x, int y, u32 color) {
  if (y >= 0 && x >= 0 && y <= SCREEN_HEIGHT-1 && x <= SCREEN_WIDTH - 1)
    state.pixels[y * SCREEN_WIDTH +  x] = color;
}

void verline(int x, int y0, int y1, u32 color) {
  for (int y = y0; y <= y1; y++) {
    drawpixel(y, x, color);
  }
}

v2 persproject(float x, float y, float z, float w) {
  float theta = state.angle;
  float far = 10;
  float near = 2;
  float dx = (1/(((float)SCREEN_WIDTH/SCREEN_HEIGHT)*tanf(theta)/2))*x;
  float dy = (1/tan(theta)/2)*y;
  float dz = (far/(far-near))*z - (far*near/(far-near))*w;
  return (v2) {
    (dx/dz),
    (dy/dz)
  };
}

void render() {

  v3 wall0 = {
    2 - state.pos.x, 2 - state.pos.y, 1 - state.pos.z
  };
  v3 wall1 = {
    wall0.x + 10 - state.pos.x, wall0.y + 10 - state.pos.y, wall0.z+0.1-state.pos.z
  };

  float tx0 = wall0.x * cos(-state.angle) + wall0.z * sin(-state.angle);
  float tz0 = wall0.z * cos(-state.angle) - wall0.x * sin(-state.angle);
  float tx1 = wall1.x * cos(-state.angle) + wall1.z * sin(-state.angle);
  float tz1 = wall1.z * cos(-state.angle) - wall1.x * sin(-state.angle);

  float distance = 400;
  float dx0 = tx0/tz0 * distance + SCREEN_WIDTH/2.0;
  float dy0 = (wall1.y - wall0.y)/tz0 * distance + SCREEN_WIDTH/2.0;
  float dx1 = tx1/tz1 * distance + SCREEN_WIDTH/2.0;
  float dy1 = (wall1.y - wall0.y)/tz1 * distance + SCREEN_WIDTH/2.0;


  verline(dx0, dy0, dy1, 0xFFFFFF);
  verline(dx1, dy0, dy1, 0xFFFFFF);

  //state.pixels[index] = 0xFFFFFF;
}


texture createtexture(const char *path) {
  SDL_Surface *s = IMG_Load(path); 
  
  u32 *arr = malloc(sizeof(u32) * s->w * s->h);
  for (int i = 0; i < s->w * s->h; i++) {
    u32 r = ((u8*)s->pixels)[i*3] << 16;
    u32 g = ((u8*)s->pixels)[i*3 + 1] << 8;
    u32 b = ((u8*)s->pixels)[i*3 + 2];
    arr[i] = r | g | b;
  }
  

  texture t  = {
    s->w,
    s->h,
    arr
  };

  SDL_FreeSurface(s);

  return t;
}

void freetexture(texture *t) {
  free(t->pixels);
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

  state.textures[0] = createtexture("./pics/greystone.png");
  
  state.angle = PI/8;
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
      state.pos.y += PI/20;
    }

    if (keystate[SDL_SCANCODE_RIGHT]) {
      state.pos.y -= PI/20;
    }

    if (keystate[SDL_SCANCODE_UP]) {
      state.angle -= PI/20;
    }

    if (keystate[SDL_SCANCODE_DOWN]) {
      state.angle += PI/20;
    }

    if (keystate[SDL_SCANCODE_S]) {
      state.pos.z -= PI/20;
    }

    if (keystate[SDL_SCANCODE_D]) {
      state.pos.x += PI/20;
    }
    if (keystate[SDL_SCANCODE_W]) {
      state.pos.z += PI/20;
    }

    if (keystate[SDL_SCANCODE_A]) {
      state.pos.x -= PI/20;
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
