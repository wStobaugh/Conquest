#include "settings_manager.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/log.h"

/* Define a max number of categories and settings per category */
#define MAX_CATEGORIES 20
#define MAX_SETTINGS_PER_CATEGORY 50
#define MAX_CALLBACKS_PER_SETTING 10

/* Callback data structure */
typedef struct {
    SettingChangedCallback callback;
    void* user_data;
} CallbackData;

/* Individual setting structure */
typedef struct Setting {
    char* key;
    char* category;
    char* display_name;
    char* description;
    SettingType type;
    SettingDisplayType display_type;
    SettingValue value;
    SettingValue default_value;
    SettingConstraint constraint;
    CallbackData callbacks[MAX_CALLBACKS_PER_SETTING];
    int callback_count;
} Setting;

/* Category structure */
typedef struct {
    char* name;
    char* display_name;
    Setting* settings[MAX_SETTINGS_PER_CATEGORY];
    int setting_count;
} Category;

/* Settings manager structure */
struct SettingsManager {
    Category categories[MAX_CATEGORIES];
    int category_count;
    char* settings_file;
};

/* Helper functions */
static Category* find_category(SettingsManager* sm, const char* category) {
    for (int i = 0; i < sm->category_count; i++) {
        if (strcmp(sm->categories[i].name, category) == 0) {
            return &sm->categories[i];
        }
    }
    return NULL;
}

static Setting* find_setting(SettingsManager* sm, const char* key) {
    for (int i = 0; i < sm->category_count; i++) {
        Category* category = &sm->categories[i];
        for (int j = 0; j < category->setting_count; j++) {
            if (strcmp(category->settings[j]->key, key) == 0) {
                return category->settings[j];
            }
        }
    }
    return NULL;
}

static void trigger_callbacks(Setting* setting) {
    for (int i = 0; i < setting->callback_count; i++) {
        setting->callbacks[i].callback(setting->key, setting->callbacks[i].user_data);
    }
}

static char* strdup_safe(const char* str) {
    if (!str) return NULL;
    char* result = malloc(strlen(str) + 1);
    if (result) {
        strcpy(result, str);
    }
    return result;
}

/* Create a new settings manager */
SettingsManager* sm_settings_create(void) {
    SettingsManager* sm = (SettingsManager*)calloc(1, sizeof(SettingsManager));
    if (!sm) {
        LOG_ERROR("Failed to allocate settings manager\n");
        return NULL;
    }
    
    return sm;
}

/* Destroy the settings manager */
void sm_settings_destroy(SettingsManager* sm) {
    if (!sm) return;
    
    // Free all categories and their settings
    for (int i = 0; i < sm->category_count; i++) {
        Category* category = &sm->categories[i];
        
        free(category->name);
        free(category->display_name);
        
        for (int j = 0; j < category->setting_count; j++) {
            Setting* setting = category->settings[j];
            
            free(setting->key);
            free(setting->category);
            free(setting->display_name);
            free(setting->description);
            
            if (setting->type == SETTING_TYPE_STRING) {
                free(setting->value.string_val);
                free(setting->default_value.string_val);
            }
            
            free(setting);
        }
    }
    
    free(sm->settings_file);
    free(sm);
}

/* Register a new category */
void sm_register_category(SettingsManager* sm, const char* category, const char* display_name) {
    if (!sm || !category) return;
    
    if (sm->category_count >= MAX_CATEGORIES) {
        LOG_ERROR("Maximum number of categories reached\n");
        return;
    }
    
    if (find_category(sm, category)) {
        LOG_ERROR("Category '%s' already exists\n", category);
        return;
    }
    
    Category* new_category = &sm->categories[sm->category_count];
    new_category->name = strdup_safe(category);
    new_category->display_name = strdup_safe(display_name ? display_name : category);
    new_category->setting_count = 0;
    
    sm->category_count++;
}

/* Common function to register a setting */
static Setting* register_setting(SettingsManager* sm, const char* category, 
                                 const char* key, const char* display_name,
                                 const char* description, SettingType type,
                                 SettingDisplayType display_type) {
    if (!sm || !category || !key) return NULL;
    
    // Make sure the category exists
    Category* cat = find_category(sm, category);
    if (!cat) {
        LOG_ERROR("Category '%s' not found\n", category);
        return NULL;
    }
    
    // Check if setting already exists
    if (find_setting(sm, key)) {
        LOG_ERROR("Setting '%s' already exists\n", key);
        return NULL;
    }
    
    // Check if we can add more settings to this category
    if (cat->setting_count >= MAX_SETTINGS_PER_CATEGORY) {
        LOG_ERROR("Maximum number of settings reached for category '%s'\n", category);
        return NULL;
    }
    
    // Create the new setting
    Setting* setting = (Setting*)calloc(1, sizeof(Setting));
    if (!setting) {
        LOG_ERROR("Failed to allocate memory for setting\n");
        return NULL;
    }
    
    setting->key = strdup_safe(key);
    setting->category = strdup_safe(category);
    setting->display_name = strdup_safe(display_name ? display_name : key);
    setting->description = strdup_safe(description);
    setting->type = type;
    setting->display_type = display_type;
    setting->callback_count = 0;
    
    // Add the setting to the category
    cat->settings[cat->setting_count++] = setting;
    
    return setting;
}

void sm_register_int_setting(SettingsManager* sm, const char* category, const char* key, 
                             const char* display_name, const char* description,
                             int default_value, int min_value, int max_value, int step,
                             SettingDisplayType display_type) {
    
    Setting* setting = register_setting(sm, category, key, display_name, 
                                       description, SETTING_TYPE_INT, display_type);
    if (!setting) return;
    
    setting->value.int_val = default_value;
    setting->default_value.int_val = default_value;
    setting->constraint.int_range.min = min_value;
    setting->constraint.int_range.max = max_value;
    setting->constraint.int_range.step = step;
}

void sm_register_float_setting(SettingsManager* sm, const char* category, const char* key, 
                              const char* display_name, const char* description,
                              float default_value, float min_value, float max_value, float step,
                              SettingDisplayType display_type) {
    
    Setting* setting = register_setting(sm, category, key, display_name, 
                                       description, SETTING_TYPE_FLOAT, display_type);
    if (!setting) return;
    
    setting->value.float_val = default_value;
    setting->default_value.float_val = default_value;
    setting->constraint.float_range.min = min_value;
    setting->constraint.float_range.max = max_value;
    setting->constraint.float_range.step = step;
}

void sm_register_bool_setting(SettingsManager* sm, const char* category, const char* key, 
                             const char* display_name, const char* description,
                             bool default_value) {
    
    Setting* setting = register_setting(sm, category, key, display_name, 
                                       description, SETTING_TYPE_BOOL, DISPLAY_TYPE_CHECKBOX);
    if (!setting) return;
    
    setting->value.bool_val = default_value;
    setting->default_value.bool_val = default_value;
}

void sm_register_string_setting(SettingsManager* sm, const char* category, const char* key, 
                               const char* display_name, const char* description,
                               const char* default_value) {
    
    Setting* setting = register_setting(sm, category, key, display_name, 
                                       description, SETTING_TYPE_STRING, DISPLAY_TYPE_TEXT_INPUT);
    if (!setting) return;
    
    setting->value.string_val = strdup_safe(default_value);
    setting->default_value.string_val = strdup_safe(default_value);
}

void sm_register_enum_setting(SettingsManager* sm, const char* category, const char* key, 
                             const char* display_name, const char* description,
                             int default_value, const char** options, int option_count) {
    
    Setting* setting = register_setting(sm, category, key, display_name, 
                                       description, SETTING_TYPE_ENUM, DISPLAY_TYPE_DROPDOWN);
    if (!setting) return;
    
    setting->value.enum_val = default_value;
    setting->default_value.enum_val = default_value;
    
    // Copy enum options
    const char** enum_options = (const char**)malloc(sizeof(const char*) * option_count);
    if (enum_options) {
        for (int i = 0; i < option_count; i++) {
            enum_options[i] = strdup_safe(options[i]);
        }
        setting->constraint.enum_options.options = enum_options;
        setting->constraint.enum_options.count = option_count;
    }
}

/* Get setting values */
int sm_get_int(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_INT) {
        LOG_ERROR("Invalid int setting: %s\n", key);
        return 0;
    }
    return setting->value.int_val;
}

float sm_get_float(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_FLOAT) {
        LOG_ERROR("Invalid float setting: %s\n", key);
        return 0.0f;
    }
    return setting->value.float_val;
}

bool sm_get_bool(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_BOOL) {
        LOG_ERROR("Invalid bool setting: %s\n", key);
        return false;
    }
    return setting->value.bool_val;
}

const char* sm_get_string(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_STRING) {
        LOG_ERROR("Invalid string setting: %s\n", key);
        return "";
    }
    return setting->value.string_val;
}

int sm_get_enum(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_ENUM) {
        LOG_ERROR("Invalid enum setting: %s\n", key);
        return 0;
    }
    return setting->value.enum_val;
}

const char* sm_get_enum_string(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_ENUM) {
        LOG_ERROR("Invalid enum setting: %s\n", key);
        return "";
    }
    
    int index = setting->value.enum_val;
    if (index >= 0 && index < setting->constraint.enum_options.count) {
        return setting->constraint.enum_options.options[index];
    }
    
    return "";
}

/* Set setting values with validation */
void sm_set_int(SettingsManager* sm, const char* key, int value) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_INT) {
        LOG_ERROR("Invalid int setting: %s\n", key);
        return;
    }
    
    // Validate the value against constraints
    if (value < setting->constraint.int_range.min) {
        value = setting->constraint.int_range.min;
    } else if (value > setting->constraint.int_range.max) {
        value = setting->constraint.int_range.max;
    }
    
    // Apply step if needed
    if (setting->constraint.int_range.step > 1) {
        value = ((value - setting->constraint.int_range.min) / 
                setting->constraint.int_range.step) * 
                setting->constraint.int_range.step + 
                setting->constraint.int_range.min;
    }
    
    // Only trigger callbacks if the value changed
    if (setting->value.int_val != value) {
        setting->value.int_val = value;
        trigger_callbacks(setting);
    }
}

void sm_set_float(SettingsManager* sm, const char* key, float value) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_FLOAT) {
        LOG_ERROR("Invalid float setting: %s\n", key);
        return;
    }
    
    // Validate the value against constraints
    if (value < setting->constraint.float_range.min) {
        value = setting->constraint.float_range.min;
    } else if (value > setting->constraint.float_range.max) {
        value = setting->constraint.float_range.max;
    }
    
    // Apply step if needed
    if (setting->constraint.float_range.step > 0) {
        value = ((int)((value - setting->constraint.float_range.min) / 
                setting->constraint.float_range.step)) * 
                setting->constraint.float_range.step + 
                setting->constraint.float_range.min;
    }
    
    if (setting->value.float_val != value) {
        setting->value.float_val = value;
        trigger_callbacks(setting);
    }
}

void sm_set_bool(SettingsManager* sm, const char* key, bool value) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_BOOL) {
        LOG_ERROR("Invalid bool setting: %s\n", key);
        return;
    }
    
    if (setting->value.bool_val != value) {
        setting->value.bool_val = value;
        trigger_callbacks(setting);
    }
}

void sm_set_string(SettingsManager* sm, const char* key, const char* value) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_STRING) {
        LOG_ERROR("Invalid string setting: %s\n", key);
        return;
    }
    
    if (!value) value = "";
    
    // Check if the value has changed
    if (strcmp(setting->value.string_val, value) != 0) {
        free(setting->value.string_val);
        setting->value.string_val = strdup_safe(value);
        trigger_callbacks(setting);
    }
}

void sm_set_enum(SettingsManager* sm, const char* key, int value) {
    Setting* setting = find_setting(sm, key);
    if (!setting || setting->type != SETTING_TYPE_ENUM) {
        LOG_ERROR("Invalid enum setting: %s\n", key);
        return;
    }
    
    // Validate enum value
    if (value < 0 || value >= setting->constraint.enum_options.count) {
        LOG_ERROR("Invalid enum value %d for setting %s\n", value, key);
        return;
    }
    
    if (setting->value.enum_val != value) {
        setting->value.enum_val = value;
        trigger_callbacks(setting);
    }
}

/* Callback registration */
void sm_register_callback(SettingsManager* sm, const char* key, 
                         SettingChangedCallback callback, void* user_data) {
    if (!sm || !key || !callback) return;
    
    Setting* setting = find_setting(sm, key);
    if (!setting) {
        LOG_ERROR("Setting not found: %s\n", key);
        return;
    }
    
    if (setting->callback_count >= MAX_CALLBACKS_PER_SETTING) {
        LOG_ERROR("Maximum callbacks reached for setting %s\n", key);
        return;
    }
    
    setting->callbacks[setting->callback_count].callback = callback;
    setting->callbacks[setting->callback_count].user_data = user_data;
    setting->callback_count++;
}

void sm_unregister_callback(SettingsManager* sm, const char* key, 
                           SettingChangedCallback callback) {
    if (!sm || !key || !callback) return;
    
    Setting* setting = find_setting(sm, key);
    if (!setting) {
        LOG_ERROR("Setting not found: %s\n", key);
        return;
    }
    
    // Find and remove the callback
    for (int i = 0; i < setting->callback_count; i++) {
        if (setting->callbacks[i].callback == callback) {
            // Move last callback to this position and decrement count
            setting->callbacks[i] = setting->callbacks[--setting->callback_count];
            break;
        }
    }
}

/* Reset functions */
void sm_reset_setting(SettingsManager* sm, const char* key) {
    Setting* setting = find_setting(sm, key);
    if (!setting) {
        LOG_ERROR("Setting not found: %s\n", key);
        return;
    }
    
    switch (setting->type) {
        case SETTING_TYPE_INT:
            sm_set_int(sm, key, setting->default_value.int_val);
            break;
        case SETTING_TYPE_FLOAT:
            sm_set_float(sm, key, setting->default_value.float_val);
            break;
        case SETTING_TYPE_BOOL:
            sm_set_bool(sm, key, setting->default_value.bool_val);
            break;
        case SETTING_TYPE_STRING:
            sm_set_string(sm, key, setting->default_value.string_val);
            break;
        case SETTING_TYPE_ENUM:
            sm_set_enum(sm, key, setting->default_value.enum_val);
            break;
    }
}

void sm_reset_category(SettingsManager* sm, const char* category) {
    if (!sm || !category) return;
    
    Category* cat = find_category(sm, category);
    if (!cat) {
        LOG_ERROR("Category not found: %s\n", category);
        return;
    }
    
    for (int i = 0; i < cat->setting_count; i++) {
        sm_reset_setting(sm, cat->settings[i]->key);
    }
}

void sm_reset_to_defaults(SettingsManager* sm) {
    if (!sm) return;
    
    for (int i = 0; i < sm->category_count; i++) {
        for (int j = 0; j < sm->categories[i].setting_count; j++) {
            sm_reset_setting(sm, sm->categories[i].settings[j]->key);
        }
    }
}

/* File I/O */
bool sm_save_settings(SettingsManager* sm, const char* filename) {
    if (!sm || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        LOG_ERROR("Failed to open settings file for writing: %s\n", filename);
        return false;
    }
    
    // Save settings format version
    fprintf(file, "# Settings file format v1.0\n");
    
    // For each category
    for (int i = 0; i < sm->category_count; i++) {
        Category* cat = &sm->categories[i];
        fprintf(file, "\n[%s]\n", cat->name);
        
        // Save each setting
        for (int j = 0; j < cat->setting_count; j++) {
            Setting* setting = cat->settings[j];
            
            // Write setting key with type
            fprintf(file, "# Type: %d\n", setting->type);
            
            // Write the value based on type
            switch (setting->type) {
                case SETTING_TYPE_INT:
                    fprintf(file, "%s=%d\n", setting->key, setting->value.int_val);
                    break;
                    
                case SETTING_TYPE_FLOAT:
                    fprintf(file, "%s=%f\n", setting->key, setting->value.float_val);
                    break;
                    
                case SETTING_TYPE_BOOL:
                    fprintf(file, "%s=%s\n", setting->key, setting->value.bool_val ? "true" : "false");
                    break;
                    
                case SETTING_TYPE_STRING:
                    fprintf(file, "%s=%s\n", setting->key, setting->value.string_val);
                    break;
                    
                case SETTING_TYPE_ENUM:
                    fprintf(file, "%s=%d\n", setting->key, setting->value.enum_val);
                    break;
            }
        }
    }
    
    fclose(file);
    return true;
}

bool sm_load_settings(SettingsManager* sm, const char* filename) {
    if (!sm || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        LOG_WARN("Settings file not found: %s (using defaults)\n", filename);
        return false;
    }
    
    char line[1024];
    char current_category[256] = "";
    int line_type = 0; // 0 = unknown, 1 = category, 2 = comment, 3 = setting
    int setting_type = -1;
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        char* nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        
        // Skip empty lines
        if (strlen(line) == 0) continue;
        
        // Determine line type
        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            // Category header
            line_type = 1;
            strncpy(current_category, line + 1, strlen(line) - 2);
            current_category[strlen(line) - 2] = '\0';
        } else if (line[0] == '#') {
            // Comment - check for type information
            line_type = 2;
            if (strncmp(line, "# Type: ", 8) == 0) {
                setting_type = atoi(line + 8);
            }
        } else {
            // Setting
            line_type = 3;
            
            // Parse key=value
            char* equals = strchr(line, '=');
            if (!equals) continue;
            
            *equals = '\0';
            char* key = line;
            char* value = equals + 1;
            
            // Find the setting
            Setting* setting = find_setting(sm, key);
            if (!setting) {
                LOG_WARN("Unknown setting '%s' in file\n", key);
                continue;
            }
            
            // Check if the setting type matches
            if (setting_type != -1 && setting_type != setting->type) {
                LOG_WARN("Type mismatch for setting '%s'\n", key);
                setting_type = -1;
                continue;
            }
            
            // Set the value based on type
            switch (setting->type) {
                case SETTING_TYPE_INT:
                    sm_set_int(sm, key, atoi(value));
                    break;
                    
                case SETTING_TYPE_FLOAT:
                    sm_set_float(sm, key, (float)atof(value));
                    break;
                    
                case SETTING_TYPE_BOOL:
                    sm_set_bool(sm, key, strcmp(value, "true") == 0);
                    break;
                    
                case SETTING_TYPE_STRING:
                    sm_set_string(sm, key, value);
                    break;
                    
                case SETTING_TYPE_ENUM:
                    sm_set_enum(sm, key, atoi(value));
                    break;
            }
            
            setting_type = -1;
        }
    }
    
    fclose(file);
    return true;
}

/* UI generation helpers */
int sm_get_category_count(SettingsManager* sm) {
    return sm ? sm->category_count : 0;
}

CategoryInfo* sm_get_categories(SettingsManager* sm) {
    if (!sm || sm->category_count == 0) return NULL;
    
    CategoryInfo* result = (CategoryInfo*)malloc(sizeof(CategoryInfo) * sm->category_count);
    if (!result) return NULL;
    
    for (int i = 0; i < sm->category_count; i++) {
        result[i].name = strdup_safe(sm->categories[i].name);
        result[i].display_name = strdup_safe(sm->categories[i].display_name);
        result[i].setting_count = sm->categories[i].setting_count;
        result[i].settings = NULL;
    }
    
    return result;
}

int sm_get_setting_count(SettingsManager* sm, const char* category) {
    if (!sm || !category) return 0;
    
    Category* cat = find_category(sm, category);
    return cat ? cat->setting_count : 0;
}

SettingInfo** sm_get_settings(SettingsManager* sm, const char* category) {
    if (!sm || !category) return NULL;
    
    Category* cat = find_category(sm, category);
    if (!cat || cat->setting_count == 0) return NULL;
    
    SettingInfo** result = (SettingInfo**)malloc(sizeof(SettingInfo*) * cat->setting_count);
    if (!result) return NULL;
    
    for (int i = 0; i < cat->setting_count; i++) {
        Setting* setting = cat->settings[i];
        
        SettingInfo* info = (SettingInfo*)malloc(sizeof(SettingInfo));
        if (!info) {
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                sm_free_setting_info(result[j]);
            }
            free(result);
            return NULL;
        }
        
        info->key = strdup_safe(setting->key);
        info->display_name = strdup_safe(setting->display_name);
        info->description = strdup_safe(setting->description);
        info->type = setting->type;
        info->display_type = setting->display_type;
        info->value = setting->value;
        info->default_value = setting->default_value;
        info->constraint = setting->constraint;
        
        result[i] = info;
    }
    
    return result;
}

SettingInfo* sm_get_setting_info(SettingsManager* sm, const char* key) {
    if (!sm || !key) return NULL;
    
    Setting* setting = find_setting(sm, key);
    if (!setting) return NULL;
    
    SettingInfo* info = (SettingInfo*)malloc(sizeof(SettingInfo));
    if (!info) return NULL;
    
    info->key = strdup_safe(setting->key);
    info->display_name = strdup_safe(setting->display_name);
    info->description = strdup_safe(setting->description);
    info->type = setting->type;
    info->display_type = setting->display_type;
    info->value = setting->value;
    info->default_value = setting->default_value;
    info->constraint = setting->constraint;
    
    return info;
}

void sm_free_category_info(CategoryInfo* info) {
    if (!info) return;
    
    free(info->name);
    free(info->display_name);
    
    if (info->settings) {
        for (int i = 0; i < info->setting_count; i++) {
            sm_free_setting_info(info->settings[i]);
        }
        free(info->settings);
    }
    
    free(info);
}

void sm_free_setting_info(SettingInfo* info) {
    if (!info) return;
    
    free(info->key);
    free(info->display_name);
    free(info->description);
    
    if (info->type == SETTING_TYPE_STRING) {
        // Don't free the string values, they're just pointers to the original
        // for UI display purposes
    }
    
    free(info);
} 