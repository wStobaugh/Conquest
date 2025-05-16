#include "menu.h"
#include "../../core/input/input_manager.h"
#include "../../core/audio/audio_manager.h"
#include "../../core/resources/resource_paths.h"
#include "../../core/resources/resource_manager.h"
#include "../../utils/math_utils.h"
#include "../../core/services/service_manager.h"
#include "../../utils/log.h"
#include "../../core/event/event_signals.h"
#include <stdio.h>
/*--------------------------------------------------------------------------*/
/* Helpers                                                                 */
/*--------------------------------------------------------------------------*/

void menu_clear_buttons(Menu *m) {
    for (int i = 0; i < m->btn_count; i++)
        button_destroy(&m->buttons[i]);
    free(m->buttons);
    m->buttons = NULL;
    m->btn_count = 0;
}

void menu_add_button(Menu *m, const char *lbl, MenuSignal signal, int y) {
    // Clamp y to the window height
    int real_y = clamp(y + m->off_y, 0, m->win_h - 90);
    m->buttons = realloc(m->buttons, sizeof(Button) * (m->btn_count + 1));
    m->buttons[m->btn_count++] =
        button_make(m->ren, m->font, lbl, signal, m->win_w / 2, real_y,
                    (SDL_Color){100, 100, 100, 255});
}
int menu_center_x(Menu *m) { return m->win_w / 2; }
void menu_get_fonts(Menu *m, void **title, void **body) {
    if (title)
        *title = m->title_font;
    if (body)
        *body = m->font;
}
/* ─────────────────────────────────────────────────────────────────────── */

// Helper function to play menu select sound
void menu_play_select_sound(Menu *m) {
    if (!m || !m->audio_manager) {
        LOG_ERROR("No menu or audio manager available\n");
        return;
    }
    
    // Get the full path to the sound file
    const char* sound_path = get_sfx_path("menu_select.wav");
    
    // Use the new one-shot function which handles all the resource management
    if (!am_play_oneshot(m->audio_manager, sound_path, 256)) {
        LOG_ERROR("Failed to play one-shot menu select sound\n");
    }
}

void menu_set_event_bus(Menu *m, EventBus *bus) {
    m->event_bus = bus;
}

Menu *menu_create(SDL_Renderer *ren, int w, int h, const char *font_path, 
                 AudioManager *audio_manager, ResourceManager *resource_manager) {
    Menu *m = calloc(1, sizeof *m);
    m->ren = ren;
    m->win_w = w;
    m->win_h = h;
    m->off_x = 0;
    m->off_y = h / 5; /* 10% offset for title */
    m->title_font = TTF_OpenFont(font_path, 64);
    m->font = TTF_OpenFont(font_path, 28);
    m->audio_manager = audio_manager;
    m->resource_manager = resource_manager;
    m->last_signal = MENU_SIGNAL_NONE;
    
    SDL_Surface *s = TTF_RenderUTF8_Solid(m->title_font, "CONQUEST",
                                          (SDL_Color){255, 255, 255, 255});
    m->title_tex = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
    
    // Use resource manager to load the background texture
    if (m->resource_manager) {
        m->bg_tex = load_texture(resource_manager, "resources/images/ui/main_bg.png", ren);
    } else {
        SDL_Log("Warning: No resource manager provided to menu. Background texture won't be loaded.");
        m->bg_tex = NULL;
    }
    
    menu_build_main(m);
    return m;
}

// Updated to emit events through the EventBus
void menu_handle_input(Menu *m, const InputManager *im) {
    int mx, my;
    input_mouse_pos(im, &mx, &my);

    // Remove or comment out debug print
    // printf("Mouse position: %d, %d\n", mx, my);

    /* 1. hover colouring */
    for (int i = 0; i < m->btn_count; ++i)
        button_hover(&m->buttons[i], mx, my);

    /* 2. click → emit signal event */
    if (input_pressed(im, ACTION_CONFIRM)) {
        for (int i = 0; i < m->btn_count; ++i) {
            if (button_hover(&m->buttons[i], mx, my)) {
                MenuSignal signal = m->buttons[i].signal;
                m->last_signal = signal;
                menu_play_select_sound(m);
                
                // Create and emit the event through the EventBus
                if (m->event_bus) {
                    // Allocate memory for the signal data that will persist
                    MenuSignal* signal_data = malloc(sizeof(MenuSignal));
                    *signal_data = signal;
                    
                    Event event = {
                        .type = EVENT_TYPE_SIGNAL,
                        .data = signal_data  // Use the allocated memory
                    };
                    bus_emit(m->event_bus, "menu_signals", &event);
                }
                
                // Call this with the enum directly
                menu_build_from_signal(m, signal);
                break;
            }
        }
    }
}

void menu_render(Menu *m, SDL_Renderer *ren) {
    if (m->bg_tex)
        SDL_RenderCopy(ren, m->bg_tex, NULL, NULL);
    else
        SDL_SetRenderDrawColor(ren, 10, 10, 30, 255), SDL_RenderClear(ren);
    int tw, th;
    SDL_QueryTexture(m->title_tex, NULL, NULL, &tw, &th);
    m->title_dst = (SDL_Rect){(m->win_w - tw) / 2, m->off_y, tw, th};
    SDL_RenderCopy(ren, m->title_tex, NULL, &m->title_dst);
    for (int i = 0; i < m->btn_count; i++)
        button_render(&m->buttons[i], ren);
}

MenuSignal menu_get_last_signal(Menu *m) {
    return m->last_signal;
}

void menu_destroy(Menu *m) {
    if (!m)
        return;
    menu_clear_buttons(m);
    if (m->title_tex)
        SDL_DestroyTexture(m->title_tex);
    if (m->bg_tex)
        SDL_DestroyTexture(m->bg_tex);
    if (m->title_font)
        TTF_CloseFont(m->title_font);
    if (m->font)
        TTF_CloseFont(m->font);
    free(m);
}

// Replace menu_pop_signal with a function that returns the enum directly
MenuSignal menu_pop_signal(Menu *m) {
    MenuSignal signal = m->last_signal;
    m->last_signal = MENU_SIGNAL_NONE;
    return signal;
}
