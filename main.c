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

#define NORMALIZE(u) ({              \
        const v2 _u = (u);           \
        const f32 l = LENGTH(_u);    \
        (v2) { _u.x / l, _u.y / l }; \
    })

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
    while (hit == 0) {
      //jump to next map square, either in x-direction, or in y-direction
      if (sideDist.x < sideDist.y) {
        sideDist.x += deltaDist.x;
        ipos.x += step.x;
        side = 0;
      }
      else {
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
    int y0 = MAX(-lineHeight / 2 + SCREEN_HEIGHT / 2, 0);
    int y1 = MIN(lineHeight / 2 + SCREEN_HEIGHT / 2, SCREEN_HEIGHT - 1);


    float wallx;
    if (side == 0) wallx = pos.y + perpWallDist * dir.y;
    else           wallx = pos.x + perpWallDist * dir.x;
    wallx -= floorf((wallx));
    
    // gets x coordiate on the wall
    int tx = (u32) (wallx * (float) state.textures[0].w);
    if (side == 0 && dir.x > 0) tx = state.textures[0].w - tx - 1;
    if (side == 1 && dir.y < 0) tx = state.textures[0].w - tx - 1;

    float s = 1.0 * state.textures[0].h / lineHeight;
    float tpos = (y0 - (float) SCREEN_HEIGHT / 2 + (float) lineHeight / 2) * s;
    for (int y = y0; y < y1; y++) {
      i32 ty = (int) tpos & (state.textures[0].h - 1);
      tpos += s;
      u32 color = state.textures[0].pixels[state.textures[0].h * ty + tx];
      if (side == 1) color = (color >> 1) & 0x7F7F7F;
      state.pixels[y * SCREEN_WIDTH + x] = color;
    }

    verline(x, 0, y0, 0x222222);
    verline(x, y1, SCREEN_HEIGHT - 1, 0x333333);

    //printf("SIDEDIST X: %f, SIDEDIST Y: %f\n", sideDist.x, sideDist.y);
    //printf("POSI X: %d, POSI Y: %d\n", ipos.x, ipos.y);
  }
}

void textureline() {

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
  
  // game
  state.pos = (v2){2, 2};
  state.dir = NORMALIZE(((v2) { -1.0f, 0.1f }));
  state.plane.x = 0; state.plane.y = 0.66;
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
