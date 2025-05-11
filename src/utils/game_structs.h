#ifndef H_GAME_STRUCTS
#define H_GAME_STRUCTS
#include <SDL2/SDL.h>

// Forward declarations
struct ServiceManager;
struct GameHandle;

typedef void (*ComputationFn)(struct GameHandle *);   /* ctx in */

// Points to a function that is called once per game loop iteration
typedef struct ComputationLayer {
    char *name; // Name of the layer
    int priority; // Priority of the layer
    ComputationFn fn; // Pointer to the computation function
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
    struct ServiceManager *services; // Service manager handle
    int running; // Flag to indicate if the game is running
} GameHandle;


#endif // H_GAME_STRUCTS