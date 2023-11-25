#pragma once

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_timer.h>
#include <GL/glext.h>

#define ASSERT(_e, ...)                                                        \
  if (!(_e)) {                                                                 \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(1);                                                                   \
  }

#define SIGN(n)                                                                \
  ({                                                                           \
    __typeof__(n) _n = (n);                                                    \
    (__typeof__(n))(_n < 0 ? -1 : (_n > 0 ? 1 : 0));                           \
  })

#define DOT(v0, v1)                                                            \
  ({                                                                           \
    const v2 _v0 = (v0), _v1 = (v1);                                           \
    (_v0.x * _v1.x) + (_v0.y * _v1.y);                                         \
  })

#define LENGTH(v)                                                              \
  ({                                                                           \
    const v2 _v = (v);                                                         \
    sqrtf(DOT(_v, _v));                                                        \
  })

#define MIN(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a), _b = (b);                                          \
    _a < _b ? _a : _b;                                                         \
  })

#define MAX(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a), _b = (b);                                          \
    _a > _b ? _a : _b;                                                         \
  })

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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_SIZE 24
#define PI 3.1415
#define FPS 144
