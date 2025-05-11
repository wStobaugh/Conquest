#ifndef COMPUTATION_STACK_H
#define COMPUTATION_STACK_H

#include "../../utils/game_structs.h"

struct GameHandle;

/* Initialize an empty stack (sets top = NULL) */
void comp_stack_init(ComputationStack *stack);

/* Add a layer to the stack (you might want to insert by priority) */
void comp_stack_add(ComputationStack *stack, ComputationLayer *layer);

/* Remove (and free) the first layer whose name matches */
void comp_stack_remove(ComputationStack *stack, const char *name);

/* Call all layers’ fn() in order */
void comp_stack_execute(const ComputationStack *stack, struct GameHandle *gh);

/* Tear down any remaining layers (frees name & structs) */
void comp_stack_destroy(ComputationStack *stack);

/* helper to allocate + push */
void push_layer(GameHandle *gh, const char *name, ComputationFn fn, int prio);

#endif // COMPUTATION_STACK_H
