#pragma once
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_surface.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#define FONT_PATH "pixeloid_font/"

class Character {
public:
  static SDL_Surface *load_character(char c) {
    std::string char_font_path_name =
        std::string(FONT_PATH) + std::string(1, c) + ".png";

    SDL_Surface *cs = IMG_Load(char_font_path_name.c_str());
    if (cs == NULL) {
      printf("ERROR\n");
    }
    return cs;
  }
};

class Font {
private:
  char characters[62] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                         'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                         'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                         'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                         'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2',
                         '3', '4', '5', '6', '7', '8', '9'};
  SDL_Texture *glyphs[sizeof(characters) / sizeof(characters[0])];
  SDL_Rect rects[sizeof(characters) / sizeof(characters[0])];

  int find_char_index(char c) {
    for (int i = 0; i < 62; i++) {
      if (c == characters[i]) {
        return i;
      }
    }

    return -1;
  }

public:
  std::string name;
  SDL_Renderer *renderer;
  Font(std::string name, SDL_Renderer *renderer)
      : name(name), renderer(renderer) {
    for (int i = 0; i < 62; i++) {
      SDL_Surface *cs = Character::load_character(characters[i]);
      glyphs[i] = SDL_CreateTextureFromSurface(renderer, cs);
      rects[i] = SDL_Rect{0, 0, cs->w, cs->h};
    }
  }

  virtual void render(std::string text, int x, int y, int s) {
    for (int c = 0; c < text.size(); c++) {
      char ch = text.at(c);
      int index = find_char_index(ch);
      if(ch == ' '){
          continue;
      }
      if (index == -1) {
        printf("ERROR: Cannot find a glyph for '%d', Skipping it...\n", ch);
        continue;
      }

      SDL_Rect dst = rects[index];
      dst.x = x + (c * dst.w / s);
      dst.y = y;
      dst.w = dst.w / s;
      dst.h = dst.h / s;
      SDL_RenderCopy(this->renderer, glyphs[index], NULL, &dst);
    }
    SDL_SetRenderDrawColor(this->renderer, 40, 40, 40, 255);
  }
};
