#include "input/input_manager.h"
#include "state/state_manager.h"
#include "game_loop/initialization.h"
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

  // Game loop initialization
  game_init();

    // 1) Create a borderless fullscreen-desktop window (size args are ignored)
    SDL_Window *win = SDL_CreateWindow(
        "Conquest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    // 2) Query the actual window (desktop) size
    int win_w, win_h;
    SDL_GetWindowSize(win, &win_w, &win_h);
    // now win_w/win_h == your monitor’s current desktop resolution

    // 3) Create the renderer
    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // 4) Pass win_w/win_h to your menu/state manager, etc.
    StateManager *sm = sm_create(ren, win_w, win_h, font_path());
    InputManager *im = input_create();

    Uint32 target_ms = 1000 / 60;
    int running = 1;
    while (running) {
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
        sm_render(sm, ren);
        SDL_RenderPresent(ren);
        input_update(im); /* wipe pressed / released for next frame */

        /* fps cap … */
        Uint32 dt = SDL_GetTicks() - frame_start;
        if (dt < target_ms)
            SDL_Delay(target_ms - dt);
    }
    /* shutdown */
    input_destroy(im);

    sm_destroy(sm);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}