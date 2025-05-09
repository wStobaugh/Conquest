#include "menu.h"
#include "../../utils/math_utils.h"

/*--------------------------------------------------------------------------*/
/* Helpers                                                                 */
/*--------------------------------------------------------------------------*/

static SDL_Texture *load_texture(SDL_Renderer *ren, const char *p) {
    SDL_Texture *t = IMG_LoadTexture(ren, p);
    if (!t)
        SDL_Log("IMG_LoadTexture(%s):%s", p, IMG_GetError());
    return t;
}

void menu_clear_buttons(Menu *m) {
    for (int i = 0; i < m->btn_count; i++)
        button_destroy(&m->buttons[i]);
    free(m->buttons);
    m->buttons = NULL;
    m->btn_count = 0;
}

void menu_add_button(Menu *m, const char *lbl, const char *sig, int y) {
    // Clamp y to the window height
    int real_y = clamp(y + m->off_y, 0, m->win_h - 90);
    m->buttons = realloc(m->buttons, sizeof(Button) * (m->btn_count + 1));
    m->buttons[m->btn_count++] =
        button_make(m->ren, m->font, lbl, sig, m->win_w / 2, real_y,
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

Menu *menu_create(SDL_Renderer *ren, int w, int h, const char *font_path) {
    Menu *m = calloc(1, sizeof *m);
    m->ren = ren;
    m->win_w = w;
    m->win_h = h;
    m->off_x = 0;
    m->off_y = h / 5; /* 10% offset for title */
    m->title_font = TTF_OpenFont(font_path, 64);
    m->font = TTF_OpenFont(font_path, 28);
    SDL_Surface *s = TTF_RenderUTF8_Solid(m->title_font, "CONQUEST",
                                          (SDL_Color){255, 255, 255, 255});
    m->title_tex = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
    m->bg_tex = load_texture(ren, "resources/images/ui/main_bg.png");
    menu_build_main(m);
    return m;
}

// Here is where we listen to ALL user input events.
void menu_handle_event(Menu *m, const SDL_Event *e) {
    // On mouse motion update button hover state
    if (e->type == SDL_MOUSEMOTION) {
        int mx = e->motion.x, my = e->motion.y;
        for (int i = 0; i < m->btn_count; i++)
            button_hover(&m->buttons[i], mx, my);
    }

    // On click...
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mx = e->button.x, my = e->button.y;
        for (int i = 0; i < m->btn_count; i++)
            if (button_hover(&m->buttons[i], mx, my)) {
                strncpy(m->signal_buf, m->buttons[i].signal,
                        sizeof m->signal_buf);
                menu_build_from_signal(
                    m, m->signal_buf); /* helper in menu_items */
                return;
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

const char *menu_pop_signal(Menu *m) {
    if (!m->signal_buf[0])
        return NULL;
    char *s = m->signal_buf;
    m->signal_buf[0] = '\0'; /* clear buffer */
    return s;
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
