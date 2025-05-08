/*
This file will contain all of the "content" of the menu, this includes
both the UI elements and logical flow of the menu.
*/
#include "menu_items.h"
#include "menu.h"
#include <stdlib.h>
#include <string.h>

static void build_buttons(Menu *m) { /* helper to reset array size */
}

void menu_build_placeholder(Menu *m, const char *title) {
    menu_clear_buttons(m);
    char txt[64];
    snprintf(txt, sizeof txt, "< %s – [Back] >", title);
    menu_add_button(m, txt, "goto_main", m->win_h - 100);
}

// main menu
void menu_build_main(Menu *m) {
    menu_clear_buttons(m);
    menu_add_button(m, "Continue", "goto_continue", 250);
    menu_add_button(m, "New Game", "goto_newgame", 310);
    menu_add_button(m, "Settings", "goto_settings", 370);
    menu_add_button(m, "Credits", "goto_credits", 430);
}

// Continue game
void menu_build_continue(Menu *m) {
    menu_clear_buttons(m);
    menu_add_button(m, "Save 1 (DEMO)", "N/A", 250);
    menu_add_button(m, "Back", "goto_main", 310);
}

// New game

// Settings

// Credits

// Public
void menu_build_from_signal(Menu *m, const char *sig) {
    if (!strcmp(sig, "goto_main"))
        menu_build_main(m);
    else if (!strcmp(sig, "goto_continue"))
        menu_build_continue(m);
    else if (!strcmp(sig, "goto_newgame"))
        menu_build_placeholder(m, "New Game");
    else if (!strcmp(sig, "goto_settings"))
        menu_build_placeholder(m, "Settings");
    else if (!strcmp(sig, "goto_credits"))
        menu_build_placeholder(m, "Credits");
}