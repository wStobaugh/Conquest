#include "default_settings.h"
#include "settings_manager.h"
#include <SDL2/SDL.h>
#include <stdio.h>

void initialize_default_settings(SettingsManager *settings) {
    if (!settings) return;
    
    // Register setting categories
    sm_register_category(settings, "video", "Video Settings");
    sm_register_category(settings, "audio", "Audio Settings");
    sm_register_category(settings, "controls", "Control Settings");
    sm_register_category(settings, "gameplay", "Gameplay Settings");
    
    // Register video settings
    sm_register_bool_setting(settings, "video", "fullscreen", 
                            "Fullscreen Mode", "Run the game in fullscreen mode", false);
    
    sm_register_int_setting(settings, "video", "resolution_w", 
                           "Resolution Width", "Screen width in pixels", 
                           1280, 800, 3840, 1, DISPLAY_TYPE_SLIDER);
    
    sm_register_int_setting(settings, "video", "resolution_h", 
                           "Resolution Height", "Screen height in pixels", 
                           720, 600, 2160, 1, DISPLAY_TYPE_SLIDER);
    
    // Register audio settings
    sm_register_float_setting(settings, "audio", "master_volume", 
                             "Master Volume", "Main volume control", 
                             1.0f, 0.0f, 1.0f, 0.1f, DISPLAY_TYPE_SLIDER);
    
    sm_register_float_setting(settings, "audio", "music_volume", 
                             "Music Volume", "Music volume level", 
                             0.8f, 0.0f, 1.0f, 0.1f, DISPLAY_TYPE_SLIDER);
    
    sm_register_float_setting(settings, "audio", "sfx_volume", 
                             "Sound Effects Volume", "Sound effects volume level", 
                             1.0f, 0.0f, 1.0f, 0.1f, DISPLAY_TYPE_SLIDER);
    
    // Register some example gameplay settings
    const char* difficulty_options[] = {"Easy", "Normal", "Hard", "Nightmare"};
    sm_register_enum_setting(settings, "gameplay", "difficulty", 
                            "Difficulty", "Game difficulty setting", 
                            1, difficulty_options, 4);
    
    // Load settings from file (if it exists)
    char* base_path = SDL_GetBasePath();
    char settings_path[512];
    snprintf(settings_path, sizeof(settings_path), "%ssettings.ini", base_path ? base_path : "");
    SDL_free(base_path);
    
    sm_load_settings(settings, settings_path);
} 