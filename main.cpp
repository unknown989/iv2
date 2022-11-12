#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_video.h"
#include <cstdio>
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include <math.h>
#include "SDL2/SDL_image.h"
#include <string>

#define max(x,y) x ? x > y : y
#define min(x,y) x ? x < y : y

SDL_Window *window;
SDL_Renderer *renderer;

int original_w,original_h;
int zoom_intensity = 90;

inline void log(const char *c) { std::printf("LOG: %s\n", c); }

SDL_Rect get_screen_size(SDL_Rect *src) {

  SDL_Rect nw{0, 0, src->w, src->h};
  if (src->w > src->h) {
    nw.h = SDL_GetWindowSurface(window)->h;
    nw.w = (int)((nw.w * nw.h) / src->h);
  } else if (src->w < src->h) {
    nw.h = (int)((nw.h * nw.w) / src->w);
    nw.w = SDL_GetWindowSurface(window)->w;
  }
  return nw;
}



int main(int argc, char **argv) {
  if (argc == 1) {
    log("Please provide an image filename :( .\n");
    return 1;
  }
  const char *filename = argv[1];
  SDL_Init(SDL_INIT_VIDEO);
  std::string title = std::string("Image Viewer | ") + std::string(filename);
  
  window =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       800, 600, SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    log("WINDOW ERROR");
    return 1;
  }
  SDL_Surface* icon = IMG_Load("assets/logo.png");
  SDL_SetWindowIcon(window, icon);
  SDL_FreeSurface(icon);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    log("RENDERER ERROR");
    return 1;
  }

  SDL_Surface *image = IMG_Load(filename);
  SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_Rect image_rect{0, 0, image->w, image->h};

  image_rect = get_screen_size(&image_rect);
  original_w = image_rect.w;
  original_h = image_rect.h;

  SDL_FreeSurface(image);

  bool running = true;
  SDL_Event ev;
  int mx,my;
  int dmx,dmy;
  bool is_mouse_down = false;
  bool is_dragging_image = false;
  while (running) {
    SDL_GetMouseState(&dmx, &dmy);
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_MOUSEBUTTONDOWN){
        is_mouse_down = true;
      }
      if (ev.type == SDL_MOUSEBUTTONUP){
        is_mouse_down = false;
        is_dragging_image = false;
      }
      
      if(is_mouse_down){
        SDL_GetMouseState(&mx,&my);
        if((image_rect.x < mx && mx < image_rect.x+image_rect.w && image_rect.y < my && my < image_rect.y + image_rect.h) || is_dragging_image){
          is_dragging_image = true;
          image_rect.x += mx -dmx;
          image_rect.y += my - dmy;
        }
      }
      if (ev.type == SDL_QUIT) {
        running = false;
      }
      if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
        case SDLK_i:{
          image_rect.w += (int)original_w/zoom_intensity+(int)(image_rect.w/original_w);
          image_rect.h += (int)original_h/zoom_intensity+(int)(image_rect.h/original_h);
          image_rect.x -= (int)original_w/(zoom_intensity*2);
          image_rect.y -= (int)original_h/(zoom_intensity*2);
          break;        
        }
        case SDLK_o:{
          image_rect.w -= (int)(original_w/zoom_intensity)-(int)(original_w/image_rect.w);
          image_rect.h -= (int)(original_h/zoom_intensity)-(int)(original_h/image_rect.h);
          image_rect.x += (int)original_w/(zoom_intensity*2);
          image_rect.y += (int)original_h/(zoom_intensity*2);
          break;
        }
        case SDLK_r:{
          image_rect.w = original_w;
          image_rect.h = original_h;
          image_rect.x = 0;
          image_rect.y = 0;
          break;
        }
        default:
          break;
        }
      }
    }
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 255);
    SDL_RenderCopy(renderer, image_texture, NULL, &image_rect);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);

  return 0;
}