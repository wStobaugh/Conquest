#ifndef CONQUEST_GUI_MENU_H
#define CONQUEST_GUI_MENU_H

#include "menu_items.h"
#include "../widgets/button.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <string.h>

/* Forward‑declared so other modules don’t see internals */
// typedef struct Menu Menu;

typedef struct Menu {
    int win_w, win_h;
    SDL_Renderer *ren;
    TTF_Font *title_font,*font;
    SDL_Texture *title_tex,*bg_tex;
    SDL_Rect title_dst;
    Button *buttons; int btn_count;
    char signal_buf[64];
} Menu;

typedef enum {
    MENU_SCREEN_MAIN,
    MENU_SCREEN_CONTINUE,
    MENU_SCREEN_NEWGAME,
    MENU_SCREEN_SETTINGS,
    MENU_SCREEN_CREDITS
} MenuScreenID;

/* lifecycle */
Menu *menu_create(SDL_Renderer *ren,int win_w,int win_h,const char *font_path);
void  menu_destroy(Menu *m);

/* frame */
void  menu_handle_event(Menu *m,const SDL_Event *e);
void  menu_render(Menu *m,SDL_Renderer *ren);
const char *menu_pop_signal(Menu *m);

/* internal helpers that content builders use */
void  menu_clear_buttons(Menu *m);
void  menu_add_button(Menu *m,const char *label,const char *signal,int y);
int   menu_center_x(Menu *m);
void  menu_get_fonts(Menu *m, void **title_font, void **body_font); /* opaque */

#endif // CONQUEST_GUI_MENU_H