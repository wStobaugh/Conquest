#include "button.h"
#include <string.h>

Button button_make(SDL_Renderer *ren, TTF_Font *font,
                   const char *text, const char *signal,
                   int center_x, int y)
{
    SDL_Color white = {255,255,255,255};
    SDL_Surface *s = TTF_RenderUTF8_Solid(font, text, white);
    if (!s) return (Button){0};
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
    if (!tex) return (Button){0};

    int w,h; SDL_QueryTexture(tex,NULL,NULL,&w,&h);
    return (Button){
        .box = { center_x - (w+20)/2, y, w+20, h+20 },
        .label = tex,
        .signal = signal
    };
}

int button_hit(const Button *b, int mx, int my)
{
    // If the label is null, return false
    if (!b->label) return 0;

    // If the signal is (null) return false
    if (!b->signal) return 0;

    // check if the mouse position is within the button box
    return (mx>=b->box.x && mx<=b->box.x+b->box.w &&
            my>=b->box.y && my<=b->box.y+b->box.h);
}

void button_render(const Button *b, SDL_Renderer *ren)
{
    SDL_SetRenderDrawColor(ren, 40,40,40,255);
    SDL_RenderFillRect(ren, &b->box);
    SDL_SetRenderDrawColor(ren, 200,200,200,255);
    SDL_RenderDrawRect(ren, &b->box);
    SDL_Rect txt_dst = { b->box.x+10, b->box.y+10,
                         b->box.w-20, b->box.h-20 };
    SDL_RenderCopy(ren, b->label, NULL, &txt_dst);
}

void button_destroy(Button *b)
{
    if (b->label) SDL_DestroyTexture(b->label);
    memset(b,0,sizeof *b);
}