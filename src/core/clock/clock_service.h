#ifndef CLOCK_SERVICE_H
#define CLOCK_SERVICE_H

#include <SDL2/SDL.h>

typedef struct {
    float last_time;
    float delta_time;
    float target_fps;
    float target_frame_time;
} ClockService;

// Initialize the clock service
ClockService *clock_service_init(void);

// Update the clock service
void clock_service_update(ClockService *clock);

// Delete the clock service
void clock_service_delete(ClockService *clock);

void clock_service_set_fps(ClockService *clock, float fps);

#endif