#include "cursor.h"
#include <SDL2/SDL_image.h>

static SDL_Cursor *c_normal = NULL;
static SDL_Cursor *c_select = NULL;

/* helper --------------------------------------------------------------- */
static SDL_Cursor *make_cursor(const char *path) {
    SDL_Surface *s = IMG_Load(path);
    if (!s) { SDL_Log("IMG_Load(%s): %s", path, IMG_GetError()); return NULL; }
    SDL_Cursor *cur = SDL_CreateColorCursor(s, 0, 0);  /* hotspot (0,0) */
    SDL_FreeSurface(s);
    return cur;
}

/* public ---------------------------------------------------------------- */
int cursor_init(void) {
    c_normal = make_cursor("resources/images/ui/cursor_normal_32.png");
    c_select = make_cursor("resources/images/ui/cursor_select_32.png");
    if (!c_normal || !c_select) return -1;
    SDL_SetCursor(c_normal);
    SDL_ShowCursor(SDL_ENABLE);
    return 0;
}

void cursor_set(CursorType t) {
    SDL_Cursor *want = (t == CURSOR_SELECT) ? c_select : c_normal;
    if (want && SDL_GetCursor() != want) SDL_SetCursor(want);
}

void cursor_cleanup(void) {
    if (c_normal)  SDL_FreeCursor(c_normal);
    if (c_select)  SDL_FreeCursor(c_select);
    c_normal = c_select = NULL;
}
