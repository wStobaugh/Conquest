#ifndef CONQUEST_GUI_MENU_H
#define CONQUEST_GUI_MENU_H

#include "../widgets/button.h"
#include "menu_items.h"
#include "../../core/event/event_bus.h"
#include "../../core/resources/resource_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
struct AudioManager;

/* Forward‑declared so other modules don't see internals */
// typedef struct Menu Menu;

typedef struct Menu {
  int win_w, win_h;
  int off_x, off_y;
  SDL_Renderer *ren;
  TTF_Font *title_font, *font;
  SDL_Texture *title_tex, *bg_tex;
  SDL_Rect title_dst;
  Button *buttons;
  int btn_count;
  MenuSignal last_signal;
  EventBus *event_bus; // Reference to the event bus
  struct AudioManager *audio_manager; // Reference to the audio manager
  ResourceManager *resource_manager; // Reference to the resource manager
} Menu;

typedef enum {
  MENU_SCREEN_MAIN,
  MENU_SCREEN_CONTINUE,
  MENU_SCREEN_NEWGAME,
  MENU_SCREEN_SETTINGS,
  MENU_SCREEN_CREDITS
} MenuScreenID;

/* lifecycle */
Menu *menu_create(SDL_Renderer *ren, int win_w, int win_h,
                  const char *font_path, struct AudioManager *audio_manager, ResourceManager *resource_manager);
void menu_destroy(Menu *m);

/* frame */
void menu_handle_event(Menu *m, const SDL_Event *e);
void menu_render(Menu *m, SDL_Renderer *ren);
MenuSignal menu_pop_signal(Menu *m);

/* internal helpers that content builders use */
void menu_clear_buttons(Menu *m);
void menu_add_button(Menu *m, const char *label, MenuSignal signal, int y);
int menu_center_x(Menu *m);
void menu_get_fonts(Menu *m, void **title_font, void **body_font); /* opaque */

// Input handling
struct InputManager;                       /* forward-declare */
void menu_handle_input(Menu *m, const struct InputManager *im);

// Sound helpers
void menu_play_select_sound(Menu *m);

// New function to set the event bus
void menu_set_event_bus(Menu *m, EventBus *bus);

// Updated function to add buttons with MenuSignal instead of string
void menu_add_button(Menu *m, const char *label, MenuSignal signal, int y);

// Remove or update the signal popping function
MenuSignal menu_get_last_signal(Menu *m);

#endif // CONQUEST_GUI_MENU_H