#include "core/input/input_manager.h"
#include "core/state/state_manager.h"
#include "game_loop/game_loop.h"
#include "game_loop/initialization.h"
#include "utils/game_structs.h"
#include "core/cursor/cursor.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static const char *font_path() {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/fonts/OpenSans-Regular.ttf",
             base ? base : "");
    SDL_free(base);
    return buf;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Game loop initialization, it reutrns a GameHandle
    GameHandle *gh = game_init();

    // Get window dimensions
    int win_w, win_h;
    SDL_GetWindowSize(gh->win, &win_w, &win_h);

    // Pass win_w/win_h to your menu/state manager, etc.
    StateManager *sm = sm_create(gh->ren, win_w, win_h, font_path());
    InputManager *im = input_create();

    Uint32 target_ms = 1000 / 60;
    int running = 1;
    while (running) {
        // TODO: Run game_loop here
        Uint32 frame_start = SDL_GetTicks();
        SDL_Event e;
        while (SDL_PollEvent(&e))
            input_handle_event(im, &e);

        /* global hot-keys */
        if (input_pressed(im, ACTION_QUIT))
            running = 0;
        /* ---------- let the state react to actions ---------------- */
        sm_handle_input(sm, im);

        sm_update(sm);
        sm_render(sm, gh->ren);
        SDL_RenderPresent(gh->ren);
        input_update(im); /* wipe pressed / released for next frame */

        /* fps cap … */
        Uint32 dt = SDL_GetTicks() - frame_start;
        if (dt < target_ms)
            SDL_Delay(target_ms - dt);
    }
    /* shutdown */

    cursor_cleanup();
    input_destroy(im);
    sm_destroy(sm);
    SDL_DestroyRenderer(gh->ren);
    SDL_DestroyWindow(gh->win);
    TTF_Quit();
    SDL_Quit();
    free(gh); /* we malloc’d it in game_init */
    return 0;
}