#include "utils.h"
#include <SDL2/SDL_render.h>

void WaitForNextFrame(int frameStart, int frame_delay) {
  int frameTime = SDL_GetTicks() - frameStart;
  if (frameTime < frame_delay)
    SDL_Delay(frame_delay - frameTime);
}

void CleanUp(SDL_Renderer *renderer, SDL_Window *window) {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}
