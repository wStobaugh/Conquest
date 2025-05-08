#include "button.h"
#include "../../utils/math_utils.h"
#include <string.h>

Button button_make(SDL_Renderer *ren, TTF_Font *font, const char *text,
                   const char *signal, int center_x, int y,
                   SDL_Color base_background_color) {
    SDL_Surface *s =
        TTF_RenderUTF8_Solid(font, text, (SDL_Color){255, 255, 255, 255});
    if (!s)
        return (Button){0};
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
    if (!tex)
        return (Button){0};

    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    return (Button){.box = {center_x - (w + 20) / 2, y, w + 20, h + 20},
                    .label = tex,
                    .signal = signal,
                    .base_background_color = base_background_color,
                    .current_background_color = base_background_color};
}

int button_hover(Button *b, int mx, int my) {
    // If the label is null, return false
    if (!b->label)
        return 0;

    // If the signal is (null) return false
    if (!b->signal)
        return 0;

    int is_hover = (mx >= b->box.x && mx <= b->box.x + b->box.w &&
                    my >= b->box.y && my <= b->box.y + b->box.h);

    // if hover is true, change the color of the button
    if (is_hover) {
        // Change the color of the button to a lighter shade
        b->current_background_color.r =
            clamp(b->base_background_color.r + 50, 0, 255);
        b->current_background_color.g =
            clamp(b->base_background_color.g + 50, 0, 255);
        b->current_background_color.b =
            clamp(b->base_background_color.b + 50, 0, 255);
    } else {
        // Reset the color of the button to its original color
        b->current_background_color.r = b->base_background_color.r;
        b->current_background_color.g = b->base_background_color.g;
        b->current_background_color.b = b->base_background_color.b;
    }
    // check if the mouse position is within the button box
    return is_hover;
}

void button_render(Button *b, SDL_Renderer *ren) {
    // Render the button with the base
    SDL_SetRenderDrawColor(
        ren, b->current_background_color.r, b->current_background_color.g,
        b->current_background_color.b, b->current_background_color.a);
    SDL_RenderFillRect(ren, &b->box);
    SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
    SDL_RenderDrawRect(ren, &b->box);
    SDL_Rect txt_dst = {b->box.x + 10, b->box.y + 10, b->box.w - 20,
                        b->box.h - 20};
    SDL_RenderCopy(ren, b->label, NULL, &txt_dst);
}

void button_destroy(Button *b) {
    if (b->label)
        SDL_DestroyTexture(b->label);
    memset(b, 0, sizeof *b);
}