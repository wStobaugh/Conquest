#include "render_service.h"
#include <stdio.h>
#include <string.h>

RenderService *renderer_init(SDL_Renderer *renderer, SDL_Window *window) {
    if (!renderer) return NULL;

    RenderService *R = malloc(sizeof(RenderService));
    if (!R) {
        printf("Failed to allocate memory for RenderService\n");
        return NULL;
    }

    // Set the renderer
    R->renderer = renderer;
    R->window = window;
    // Set default render properties
    SDL_SetRenderDrawBlendMode(R->renderer, SDL_BLENDMODE_BLEND);
    
    // Initialize layer system
    R->layer_count = 0;
    memset(R->layers, 0, sizeof(R->layers));
    
    return R;
}

void renderer_shutdown(RenderService *R) {
    if (!R) return;

    if (R->renderer) {
        SDL_DestroyRenderer(R->renderer);
        R->renderer = NULL;
    }

    if (R->window) {
        SDL_DestroyWindow(R->window);
        R->window = NULL;
    }

    // Clear layers
    R->layer_count = 0;
    memset(R->layers, 0, sizeof(R->layers));

    free(R);
}

void renderer_begin_frame(RenderService *R) {
    if (!R || !R->renderer) return;

    // Clear the screen with a default color (black)
    SDL_SetRenderDrawColor(R->renderer, 0, 0, 0, 255);
    SDL_RenderClear(R->renderer);
}

void renderer_present(RenderService *R)
{
    if (!R || !R->renderer) return;

    for (int i = 0; i < R->layer_count; ++i) {
        RenderLayer *L = &R->layers[i];
        if (L->render_func)
            L->render_func(R->renderer, L->userdata);
    }
    SDL_RenderPresent(R->renderer);
}

int renderer_add_layer(RenderService *R,
                       RenderFunc     fn,
                       void          *userdata,
                       const char    *name)
{
    if (!R || !fn || R->layer_count >= MAX_RENDER_LAYERS) return -1;
    RenderLayer *L   = &R->layers[R->layer_count++];
    L->render_func   = fn;
    L->userdata      = userdata;          /* NEW */
    strncpy(L->name, name, sizeof L->name - 1);
    return R->layer_count - 1;
}

void renderer_remove_layer_index(RenderService *R, int index) {
    if (!R || index < 0 || index >= R->layer_count) {
        return;
    }

    // Shift remaining layers down
    for (int i = index; i < R->layer_count - 1; i++) {
        R->layers[i] = R->layers[i + 1];
    }
    
    // Clear the last layer and decrement count
    R->layers[R->layer_count - 1].render_func = NULL;
    R->layers[R->layer_count - 1].name[0] = '\0';
    R->layer_count--;
}

void renderer_remove_layer_name(RenderService *R, const char *name) {
    for (int i = 0; i < R->layer_count; i++) {
        if (strcmp(R->layers[i].name, name) == 0) {
            renderer_remove_layer_index(R, i);
            return;  // Only remove the first matching layer
        }
    }
}

void renderer_remove_all_layers(RenderService *R) {
    if (!R) return;
    R->layer_count = 0;
    memset(R->layers, 0, sizeof(R->layers));
}

bool renderer_insert_layer(RenderService *R, RenderFunc layer, const char *name, int position) {
    if (!R || !layer || position < 0 || position > R->layer_count || R->layer_count >= MAX_RENDER_LAYERS) {
        return false;
    }

    // Shift layers up to make space
    for (int i = R->layer_count; i > position; i--) {
        R->layers[i] = R->layers[i - 1];
    }

    // Insert the new layer
    R->layers[position].render_func = layer;
    strncpy(R->layers[position].name, name, sizeof(R->layers[position].name) - 1);
    R->layers[position].name[sizeof(R->layers[position].name) - 1] = '\0';
    R->layer_count++;
    
    return true;
}
