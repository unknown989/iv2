#pragma once

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_surface.h"
#include "config.hpp"
#include "font.hpp"
#include <string>

class Component {
protected:
  SDL_Renderer *renderer;
  SDL_Surface *surf;
  SDL_Rect container;
  SDL_Texture* texture;
  int mx, my;
  Uint32 buttons;

public:
  Component(SDL_Renderer *renderer, int x, int y, int w, int h) {
    this->renderer = renderer;
    this->container = SDL_Rect{x, y, w, h};
  }

  bool isLeftClicked(SDL_Event *ev) {
    if (isMouseInside(ev) && ((this->buttons & SDL_BUTTON_LMASK) != 0)) {
      return true;
    }
    return false;
  }
  bool isRightClicked(SDL_Event *ev) {
    if (isMouseInside(ev) && ((this->buttons & SDL_BUTTON_RMASK) != 0)) {
      return true;
    }
    return false;
    
  }
  bool isMouseInside(SDL_Event *ev) {
    this->buttons = SDL_GetMouseState(&this->mx, &this->my);
    if (this->container.x < this->mx &&
        this->mx < this->container.x + this->container.w &&
        this->container.y < this->my &&
        this->my < this->container.y + this->container.h) {
      return true;
    }
    return false;
  }
  bool isMouseOutside(SDL_Event *ev) { return !isMouseInside(ev); }
  
  
  virtual void render(){
      SDL_RenderCopy(this->renderer, this->texture, NULL, &this->container);
  }
};

class Button : public Component {
public:
  std::string text;
  Font pixeloid = Font("Pixeloid",renderer);
  Uint32 color;
  int s;
  Button(SDL_Renderer *renderer, int x, int y, int w, int h, std::string text,
         Uint32 color,int s)
      : Component(renderer, x, y, w, h), text{text}, color{color}, s(s) {
          this->surf = SDL_CreateRGBSurface(0, this->container.w, this->container.h, 32, 0,0,0,0);
          SDL_FillRect(this->surf, NULL,this->color);
          this->texture = SDL_CreateTextureFromSurface(this->renderer, this->surf);
      }
  void render() override{
    if(can_draw_text){
      SDL_RenderCopy(this->renderer, this->texture, NULL, &this->container);
      this->pixeloid.render(this->text, this->container.x + 10, this->container.y, this->s);
    }
  }
};