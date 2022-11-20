#include "SDL2/SDL_video.h"
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "font.hpp"
#include "internet.hpp"
#include "ui.hpp"
#include <cstdio>
#include <math.h>
#include <string>

#include <regex>

bool validate_url(std::string url) {
  std::regex url_regex("\\b((?:https?|ftp|file)://[-a-zA-Z0-9+&@#/%?=~_|!:, "
                       ".;]*[-a-zA-Z0-9+&@#/%=~_|])");

  return std::regex_match(url, url_regex);
}

// Add ability to use URLs and Local files

SDL_Window *window;
SDL_Renderer *renderer;

int original_w, original_h;
int zoom_intensity = 90;
bool show_gui = true;
bool show_instructions = true;

inline void log(const char *c) { std::printf("LOG: %s\n", c); }

SDL_Rect get_screen_size(SDL_Rect *src) {

  SDL_Rect nw{0, 0, src->w, src->h};
  // if (src->w < 800 && src->h < 600) {
  //   return nw;
  // }
  float sw = (float)SDL_GetWindowSurface(window)->w / src->h;
  float sh = (float)SDL_GetWindowSurface(window)->h / src->w;

  nw.w = src->w * sw;
  nw.h = src->h * sh;

  return nw;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    log("Please provide an image filename or URL :( .\n");
    return 1;
  }
  char *filename = argv[1];
  if (validate_url(filename)) {
    DownloadURLImage(filename, NULL, (char *)"tmp.jpg");
    filename = (char *)"./tmp.jpg";
  }

  SDL_Init(SDL_INIT_VIDEO);
  std::string title = std::string("Image Viewer | ") + std::string(filename);

  window =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    log("WINDOW ERROR");
    return 1;
  }
  SDL_Surface *icon = IMG_Load("assets/logo.png");
  SDL_SetWindowIcon(window, icon);
  SDL_FreeSurface(icon);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    log("RENDERER ERROR");
    return 1;
  }

  SDL_Surface *image = IMG_Load(filename);
  if (image == nullptr) {
    log("Failed to load Image, quitting...\n");
    exit(1);
  }
  SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_Rect image_rect{0, 0, image->w, image->h};

  image_rect = get_screen_size(&image_rect);
  original_w = image_rect.w;
  original_h = image_rect.h;

  SDL_FreeSurface(image);

  bool running = true;
  SDL_Event ev;
  int mx, my;
  int dmx, dmy;
  bool is_mouse_down = false;
  bool is_dragging_image = false;

  test_path();

  Button reset_button(renderer, 10, 10, 120, 40, "Reset", 0xFF00FF00, 15);
  Button zoomin_button(renderer, 10, 60, 120, 40, "Zoom In", 0xFFFFFFFF, 15);
  Button zoomout_button(renderer, 10, 110, 120, 40, "Zoom Out", 0xFF0055FF, 15);
  Font pixeloid("Pixeloid", renderer);
  while (running) {
    SDL_GetMouseState(&dmx, &dmy);
    while (SDL_PollEvent(&ev)) {
      // UI Related Events
      if (show_gui) {

        if (reset_button.isLeftClicked(&ev)) {
          image_rect.w = original_w;
          image_rect.h = original_h;
          image_rect.x = 0;
          image_rect.y = 0;
        }
        if (zoomin_button.isLeftClicked(&ev)) {
          image_rect.w += (int)(image_rect.w / zoom_intensity);
          image_rect.h += (int)(image_rect.h / zoom_intensity);
          image_rect.x -= (int)original_w / (zoom_intensity * 2);
          image_rect.y -= (int)original_h / (zoom_intensity * 2);
        }
        if (zoomout_button.isLeftClicked(&ev)) {
          image_rect.w -= (int)(image_rect.w / zoom_intensity);
          image_rect.h -= (int)(image_rect.h / zoom_intensity);
          image_rect.x += (int)image_rect.w / (zoom_intensity * 2);
          image_rect.y += (int)image_rect.h / (zoom_intensity * 2);
        }
      }

      // End

      if (ev.type == SDL_MOUSEBUTTONDOWN) {
        is_mouse_down = true;
      }
      if (ev.type == SDL_MOUSEBUTTONUP) {
        is_mouse_down = false;
        is_dragging_image = false;
      }

      if (is_mouse_down) {
        SDL_GetMouseState(&mx, &my);
        if ((image_rect.x < mx && mx < image_rect.x + image_rect.w &&
             image_rect.y < my && my < image_rect.y + image_rect.h) ||
            is_dragging_image) {
          is_dragging_image = true;
          image_rect.x += mx - dmx;
          image_rect.y += my - dmy;
        }
      }
      if (ev.type == SDL_QUIT) {
        running = false;
      }
      if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
        case SDLK_i: {
          image_rect.w += (int)(image_rect.w / zoom_intensity);
          image_rect.h += (int)(image_rect.h / zoom_intensity);
          image_rect.x -= (int)original_w / (zoom_intensity * 2);
          image_rect.y -= (int)original_h / (zoom_intensity * 2);
          break;
        }
        case SDLK_o: {
          image_rect.w -= (int)(image_rect.w / zoom_intensity);
          image_rect.h -= (int)(image_rect.h / zoom_intensity);
          image_rect.x += (int)image_rect.w / (zoom_intensity * 2);
          image_rect.y += (int)image_rect.h / (zoom_intensity * 2);
          break;
        }
        case SDLK_r: {
          image_rect.w = original_w;
          image_rect.h = original_h;
          image_rect.x = 0;
          image_rect.y = 0;
          break;
        }
        case SDLK_h: {
          show_gui = !show_gui;
          show_instructions = false;
        }
        default:
          break;
        }
      }
    }
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderCopy(renderer, image_texture, NULL, &image_rect);
    if (show_gui) {

      reset_button.render();
      zoomin_button.render();
      zoomout_button.render();
      if (show_instructions) {
        pixeloid.render("Press R to Reset", 10, 400, 10);
        pixeloid.render("Press I to Zoom In", 10, 500, 10);
        pixeloid.render("Press O to Zoom Out", 10, 450, 10);
        pixeloid.render("Press H to hide GUI", 10, 550, 10);
      }
    }
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);

  return 0;
}