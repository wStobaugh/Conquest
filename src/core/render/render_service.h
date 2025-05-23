#ifndef CONQUEST_RENDER_SERVICE_H
#define CONQUEST_RENDER_SERVICE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_RENDER_LAYERS 32

typedef void (*RenderFunc)(SDL_Renderer *ren, void *userdata);

typedef struct {
    RenderFunc render_func;
    void      *userdata;      /* NEW */
    char       name[32];
} RenderLayer;
typedef struct RenderService {
    SDL_Renderer *renderer;
    SDL_Window *window;
    RenderLayer layers[MAX_RENDER_LAYERS];
    int layer_count;
} RenderService;

// Core initialization and shutdown
RenderService *renderer_init(SDL_Renderer *renderer, SDL_Window *window);
void renderer_shutdown(RenderService *R);

// Frame management
void renderer_begin_frame(RenderService *R);
void renderer_present(RenderService *R);

// Layer management
int renderer_add_layer(RenderService *R,
                       RenderFunc     fn,
                       void          *userdata,
                       const char    *name);
void renderer_remove_layer_index(RenderService *R, int index);
void renderer_remove_layer_name(RenderService *R, const char *name);
void renderer_remove_all_layers(RenderService *R);
bool renderer_insert_layer(RenderService *R, RenderFunc layer, const char *name, int position);

#endif // CONQUEST_RENDER_SERVICE_H
