#include "clock_service.h"
#include <SDL2/SDL.h>
#include <stdio.h>

// Initialize the clock service
ClockService *clock_service_init(void) {
    ClockService *clock = (ClockService *)malloc(sizeof(ClockService));
    if (clock == NULL) {
        return NULL;
    }
    
    clock->last_time = SDL_GetTicks();
    clock->delta_time = 0.0f;
    clock->target_fps = 60.0f;  // Default to 60 FPS
    clock->target_frame_time = 1000.0f / 60.0f;  // Convert to milliseconds
    
    return clock;
}

// Set the target FPS
void clock_service_set_fps(ClockService *clock, float fps) {
    if (clock == NULL || fps <= 0.0f) {
        return;
    }
    clock->target_fps = fps;
    clock->target_frame_time = 1000.0f / fps;
}

// Update the clock service
void clock_service_update(ClockService *clock) {
    if (clock == NULL) {
        return;
    }
    
    float current_time = SDL_GetTicks();
    clock->delta_time = (current_time - clock->last_time) / 1000.0f;
    clock->last_time = current_time;

    // Handle FPS cap
    float frame_time = SDL_GetTicks() - current_time;
    if (frame_time < clock->target_frame_time) {
        SDL_Delay((Uint32)(clock->target_frame_time - frame_time));
    }
    // print the FPS
    printf("FPS: %f\n", 1.0f / clock->delta_time);
}

// Delete the clock service
void clock_service_delete(ClockService *clock) {
    if (clock != NULL) {
        free(clock);
    }
}