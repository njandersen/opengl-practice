#pragma once
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <SDL2/SDL.h>

class RenderWindow {

public:
  RenderWindow(const char *title, int width, int height);
  ~RenderWindow();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
};
