#ifndef CONQUEST_GUI_BUTTON_H
#define CONQUEST_GUI_BUTTON_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/** Lightweight clickable button */
typedef struct Button {
    SDL_Rect     box;
    SDL_Texture *label;       /* text rendered into texture */
    const char  *signal;      /* event string to send on click */
} Button;

Button  button_make(SDL_Renderer *ren, TTF_Font *font,
                    const char *text, const char *signal,
                    int center_x, int y);
int     button_hit(const Button *b, int mx, int my);
void    button_render(const Button *b, SDL_Renderer *ren);
void    button_destroy(Button *b);
#endif