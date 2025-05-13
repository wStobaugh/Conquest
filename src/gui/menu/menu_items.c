/*
This file will contain all of the "content" of the menu, this includes
both the UI elements and logical flow of the menu.
*/
#include "menu_items.h"
#include "menu.h"
#include "../../core/event/event_signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menu_build_placeholder(Menu *m, const char *title) {
    menu_clear_buttons(m);
    char txt[64];
    snprintf(txt, sizeof txt, "< %s – [Back] >", title);
    menu_add_button(m, txt, MENU_SIGNAL_GOTO_MAIN, m->win_h - 100);
}

// main menu
void menu_build_main(Menu *m) {
    menu_clear_buttons(m);
    menu_add_button(m, "Continue", MENU_SIGNAL_GOTO_CONTINUE, 200);
    menu_add_button(m, "New Game", MENU_SIGNAL_GOTO_NEW_GAME, 260);
    menu_add_button(m, "Options", MENU_SIGNAL_GOTO_OPTIONS, 320);
    menu_add_button(m, "Quit", MENU_SIGNAL_GOTO_QUIT, 380);
}

// Continue game
void menu_build_continue(Menu *m) {
    menu_clear_buttons(m);
    int base_y = m->win_h * 0.2;
    int spacing = 70;
    menu_add_button(m, "Save 1 (DEMO)", MENU_SIGNAL_NONE, base_y);
    menu_add_button(m, "Back", MENU_SIGNAL_GOTO_MAIN, base_y + spacing);
}

// New game

// Settings

// Credits

// Public
void menu_build_from_signal(Menu *m, MenuSignal signal) {
    switch (signal) {
        case MENU_SIGNAL_GOTO_CONTINUE:
            menu_build_placeholder(m, "Continue Game");
            break;
        case MENU_SIGNAL_GOTO_NEW_GAME:
            menu_build_placeholder(m, "New Game");
            break;
        case MENU_SIGNAL_GOTO_OPTIONS:
            menu_build_placeholder(m, "Options");
            break;
        case MENU_SIGNAL_GOTO_MAIN:
            menu_build_main(m);
            break;
        case MENU_SIGNAL_GOTO_QUIT:
            // Handled by state manager
            break;
        default:
            break;
    }
}