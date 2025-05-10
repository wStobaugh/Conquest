/*
This file is part of the game engine project.


*/
#include "game_loop.h"
#include "../utils/game_structs.h"
#include <SDL2/SDL.h>

// Runs one iteration of the game loop
void game_loop(GameHandle *gh) {
    // Iterate though a list of computation layers
}

// Function to add a computation layer to the stack
void add_computation_layer(ComputationStack *stack, ComputationLayer *layer) {
    layer->next = stack->top;
    stack->top = layer;
}
// Function to remove a computation layer from the stack given its name
void remove_computation_layer(ComputationStack *stack, const char *name) {
    ComputationLayer *current = stack->top;
    ComputationLayer *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (previous == NULL) {
                stack->top = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

// Function to execute all computation layers in the stack
static void execute_computation_layers(ComputationStack *stack) {
    ComputationLayer *current = stack->top;
    while (current != NULL) {
        current->fn();
        current = current->next;
    }
}