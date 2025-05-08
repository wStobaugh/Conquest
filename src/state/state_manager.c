#include "state_manager.h"
#include <string.h>

// This is how we initialize a new game state manager.
StateManager *sm_create(SDL_Renderer *ren,int w,int h,const char *font_path)
{
    StateManager *sm = calloc(1,sizeof *sm);
    sm->state = GS_MENU;
    sm->menu = menu_create(ren,w,h,font_path);
    return sm;
}


// This function handles events for the state manager.
void sm_handle_event(StateManager *sm,const SDL_Event *e)
{
    if(sm->state==GS_MENU) menu_handle_event(sm->menu,e);
    else if(sm->state==GS_PLAY) {
        if(e->type==SDL_KEYDOWN && e->key.keysym.sym==SDLK_ESCAPE)
            sm->state = GS_MENU; /* simple toggle */
    }
}


// This function updates the state manager.
void sm_update(StateManager *sm)
{
    if(sm->state==GS_MENU){
        const char *sig = menu_pop_signal(sm->menu);
        if(!sig) return;
        if(strcmp(sig,"goto_newgame")==0 || strcmp(sig,"goto_continue")==0){
            sm->state = GS_PLAY; /* TODO: load or create world */
        }
    }
}


// This function renders the current state of the state manager.
void sm_render(StateManager *sm,SDL_Renderer *ren)
{
    if(sm->state==GS_MENU) menu_render(sm->menu,ren);
    else if(sm->state==GS_PLAY){
        SDL_SetRenderDrawColor(ren,0,20,40,255);
        SDL_RenderClear(ren);
    }
}


// This function destroys the state manager and frees its resources.
void sm_destroy(StateManager *sm)
{
    if(!sm) return;
    menu_destroy(sm->menu);
    free(sm);
}
