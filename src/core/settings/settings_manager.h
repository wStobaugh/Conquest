#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

/* Forward declaration */
typedef struct SettingsManager SettingsManager;

/* Setting types */
typedef enum {
    SETTING_TYPE_INT,
    SETTING_TYPE_FLOAT,
    SETTING_TYPE_BOOL,
    SETTING_TYPE_STRING,
    SETTING_TYPE_ENUM
} SettingType;

/* Setting display type for UI generation */
typedef enum {
    DISPLAY_TYPE_SLIDER,
    DISPLAY_TYPE_CHECKBOX,
    DISPLAY_TYPE_TEXT_INPUT,
    DISPLAY_TYPE_DROPDOWN,
    DISPLAY_TYPE_BUTTON
} SettingDisplayType;

/* Setting constraint types */
typedef struct {
    union {
        struct { int min; int max; int step; } int_range;
        struct { float min; float max; float step; } float_range;
        struct { const char** options; int count; } enum_options;
    };
} SettingConstraint;

/* Setting change callback */
typedef void (*SettingChangedCallback)(const char* key, void* user_data);

/* Setting value */
typedef union {
    int int_val;
    float float_val;
    bool bool_val;
    char* string_val;
    int enum_val;
} SettingValue;

/* Create the settings manager singleton */
SettingsManager* sm_settings_create(void);

/* Destroy the settings manager */
void sm_settings_destroy(SettingsManager* sm);

/* Register setting categories */
void sm_register_category(SettingsManager* sm, const char* category, const char* display_name);

/* Register setting with metadata */
void sm_register_int_setting(SettingsManager* sm, const char* category, const char* key, 
                             const char* display_name, const char* description,
                             int default_value, int min_value, int max_value, int step,
                             SettingDisplayType display_type);

void sm_register_float_setting(SettingsManager* sm, const char* category, const char* key, 
                               const char* display_name, const char* description,
                               float default_value, float min_value, float max_value, float step,
                               SettingDisplayType display_type);

void sm_register_bool_setting(SettingsManager* sm, const char* category, const char* key, 
                              const char* display_name, const char* description,
                              bool default_value);

void sm_register_string_setting(SettingsManager* sm, const char* category, const char* key, 
                                const char* display_name, const char* description,
                                const char* default_value);

void sm_register_enum_setting(SettingsManager* sm, const char* category, const char* key, 
                              const char* display_name, const char* description,
                              int default_value, const char** options, int option_count);

/* Get setting values */
int sm_get_int(SettingsManager* sm, const char* key);
float sm_get_float(SettingsManager* sm, const char* key);
bool sm_get_bool(SettingsManager* sm, const char* key);
const char* sm_get_string(SettingsManager* sm, const char* key);
int sm_get_enum(SettingsManager* sm, const char* key);
const char* sm_get_enum_string(SettingsManager* sm, const char* key);

/* Set setting values */
void sm_set_int(SettingsManager* sm, const char* key, int value);
void sm_set_float(SettingsManager* sm, const char* key, float value);
void sm_set_bool(SettingsManager* sm, const char* key, bool value);
void sm_set_string(SettingsManager* sm, const char* key, const char* value);
void sm_set_enum(SettingsManager* sm, const char* key, int value);

/* Register/unregister callbacks for setting changes */
void sm_register_callback(SettingsManager* sm, const char* key, 
                          SettingChangedCallback callback, void* user_data);
void sm_unregister_callback(SettingsManager* sm, const char* key, 
                            SettingChangedCallback callback);

/* Save/load settings to/from disk */
bool sm_save_settings(SettingsManager* sm, const char* filename);
bool sm_load_settings(SettingsManager* sm, const char* filename);

/* Reset settings */
void sm_reset_to_defaults(SettingsManager* sm);
void sm_reset_setting(SettingsManager* sm, const char* key);
void sm_reset_category(SettingsManager* sm, const char* category);

/* Utility functions for UI generation */
typedef struct {
    char* key;
    char* display_name;
    char* description;
    SettingType type;
    SettingDisplayType display_type;
    SettingValue value;
    SettingValue default_value;
    SettingConstraint constraint;
} SettingInfo;

typedef struct {
    char* name;
    char* display_name;
    int setting_count;
    SettingInfo** settings;
} CategoryInfo;

/* Get information about settings for UI generation */
int sm_get_category_count(SettingsManager* sm);
CategoryInfo* sm_get_categories(SettingsManager* sm);
int sm_get_setting_count(SettingsManager* sm, const char* category);
SettingInfo** sm_get_settings(SettingsManager* sm, const char* category);
SettingInfo* sm_get_setting_info(SettingsManager* sm, const char* key);

/* Free memory allocated for UI generation info */
void sm_free_category_info(CategoryInfo* info);
void sm_free_setting_info(SettingInfo* info);

#endif /* SETTINGS_MANAGER_H */ 