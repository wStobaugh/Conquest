#ifndef CONQUEST_GUI_BUTTON_H
#define CONQUEST_GUI_BUTTON_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/** Lightweight clickable button */
typedef struct Button {
  SDL_Rect box;
  SDL_Texture *label; /* text rendered into texture */
  SDL_Color base_background_color; /* color of the button */
  SDL_Color current_background_color; /* color of the button */
  const char *signal; /* event string to send on click */
} Button;

Button button_make(SDL_Renderer *ren, TTF_Font *font, const char *text,
                   const char *signal, int center_x, int y,
                   SDL_Color base_background_color);
int button_hover(Button *b, int mx, int my);
void button_render(Button *b, SDL_Renderer *ren);
void button_destroy(Button *b);
#endif