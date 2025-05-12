#ifndef COMPUTATION_LAYERS_H
#define COMPUTATION_LAYERS_H

#include "../../utils/game_structs.h"

/* Layer for handling input in the state manager */
void layer_state_input(GameHandle *gh);

/* Layer for updating the state manager */
void layer_state_update(GameHandle *gh);

/* Layer for rendering the state manager */
void layer_state_render(GameHandle *gh);

/* Layer for presenting the rendered frame */
void layer_present(GameHandle *gh);

/* Register all standard computation layers to the game handle */
void register_standard_layers(GameHandle *gh);

#endif /* COMPUTATION_LAYERS_H */ 