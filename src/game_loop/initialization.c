#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>
#include "initialization.h"
#include "../core/cursor/cursor.h"
#include "../core/services/service_manager.h"


// Set mouse image
static const char *mouse_path() {
    char *base = SDL_GetBasePath();
    static char buf[1024];
    snprintf(buf, sizeof buf, "%sresources/images/ui/cursor_normal.png",
             base ? base : "");
    SDL_free(base);
    return buf;
}

static void SDL_CheckErrors() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return;
    }
    if (TTF_Init() != 0) {
        SDL_Log("TTF: %s", TTF_GetError());
        SDL_Quit();
        return;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        SDL_Quit();
        return;
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n",
                IMG_GetError());
        SDL_Quit();
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                TTF_GetError());
        SDL_Quit();
        return;
    }
}

// entry function
GameHandle *game_init(void) {
    // Initialize SDL & check for errors
    SDL_CheckErrors();

    // Set the working directory to the base path of game
    char *base = SDL_GetBasePath();
    chdir(base);
    SDL_free(base);

    // 1) Create a borderless fullscreen-desktop window (size args are ignored)
    SDL_Window *win = SDL_CreateWindow(
        "Conquest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    // 2) Create the renderer
    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* allocate and populate the handle */
    GameHandle *gh = malloc(sizeof *gh);
    if (!gh) {
        SDL_Log("Out-of-memory in game_init");
        return NULL;
    }

    gh->win = win;
    gh->ren = ren;
    gh->services = svc_create();
    gh->stack = malloc(sizeof(ComputationStack));

    // TODO: populate the stack with computation layers

    // Initialise cursor module (ignore return but log if needed)
    if (cursor_init() != 0)
        SDL_Log("cursor_init() failed – using default system cursor");

    return gh;
}
