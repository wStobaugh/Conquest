#ifndef COMPUTATION_LAYERS_H
#define COMPUTATION_LAYERS_H

#include "../../utils/game_structs.h"

/* Layer priority constants */
#define LAYER_PRIORITY_PRESENT 0      /* Present frame and update input */
#define LAYER_PRIORITY_CLOCK 0        /* Update game clock */
#define LAYER_PRIORITY_RENDER 100     /* Render game state */
#define LAYER_PRIORITY_INPUT 300      /* Handle input processing */

/* Layer for handling input in the state manager */
void layer_state_input(GameHandle *gh);

/* Layer for rendering the state manager */
void layer_state_render(GameHandle *gh);

/* Layer for presenting the rendered frame */
void layer_present(GameHandle *gh);

/* Register all standard computation layers to the game handle */
void register_standard_layers(GameHandle *gh);

#endif /* COMPUTATION_LAYERS_H */ 