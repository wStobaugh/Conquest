#ifndef H_GAME_STRUCTS
#define H_GAME_STRUCTS
#include <SDL2/SDL.h>

// Points to a function that is called once per game loop iteration
typedef struct ComputationLayer {
    char *name; // Name of the layer
    int priority; // Priority of the layer
    void (*fn)(void); // Pointer to the computation function
    struct ComputationLayer *next; // Pointer to the next layer
} ComputationLayer;

// A linked list of computation layers
typedef struct ComputationStack {
    ComputationLayer *top; // Pointer to the top of the stack
} ComputationStack;

// Handle to the game
typedef struct GameHandle {
    SDL_Window *win;   // Window handle
    SDL_Renderer *ren; // Renderer handle
    ComputationStack *stack; // Stack of computation layers
} GameHandle;


#endif // H_GAME_STRUCTS