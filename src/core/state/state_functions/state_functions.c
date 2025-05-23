#include "state_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Create vtable for each state
static int NUM_STATES = 4;
static StateVTable MENU_VTABLE = {
  menu_state_enter,
  menu_state_update,
  menu_state_exit
};

static StateVTable PLAY_VTABLE = {
  play_state_enter,
  play_state_update,
  play_state_exit
};

static StateVTable PAUSE_VTABLE = {
  pause_state_enter,
  pause_state_update,
  pause_state_exit
};

static StateVTable QUIT_VTABLE = {
  quit_state_enter,
  NULL,
  NULL
};


// Function to create a StateVTable
static StateVTable *create_state_vtable(void* enter, void* update, void* exit) {
  StateVTable *vtable = (StateVTable *)malloc(sizeof(StateVTable));
  vtable->enter = enter;
  vtable->update = update;
  vtable->exit = exit;
  return vtable;
}


void register_game_state(GameStates *states, StateVTable *vtable, enum GameState type, int index) {
    states->states[index] = (GameStateObject) {
        .vtable = vtable,
        .type = type
    };
}

GameStates *init_game_states(void) {
    GameStates *states = (GameStates *)malloc(sizeof(GameStates));
    states->states = (GameStateObject *)malloc(sizeof(GameStateObject) * NUM_STATES);
    states->num_states = NUM_STATES;

    register_game_state(states, &MENU_VTABLE, GS_MENU, 0);
    register_game_state(states, &PLAY_VTABLE, GS_PLAY, 1);
    register_game_state(states, &QUIT_VTABLE, GS_QUIT, 2);
    register_game_state(states, &PAUSE_VTABLE, GS_PAUSE, 3);

    return states;
}

// return the table for the given state
StateVTable *get_state_vtable(GameStates *states, enum GameState state) {
    return states->states[state].vtable;
}

// return the object for the given state
GameStateObject *get_state_object(GameStates *states, enum GameState state) {
    assert(states && "get_state_object: states == NULL");
    assert(state < states->num_states && "get_state_object: state out of bounds");
    return &states->states[state];
}