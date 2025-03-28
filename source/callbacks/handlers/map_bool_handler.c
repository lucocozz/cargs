#define _GNU_SOURCE  // NOLINT

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/options.h"
#include "cargs/types.h"

/**
 * Convert a string to a boolean value
 * Accepts various representations of boolean values
 *
 * True: "true", "yes", "1", "on", "y" (case-insensitive)
 * False: "false", "no", "0", "off", "n" (case-insensitive)
 *
 * @param value String to convert
 * @param result Pointer to store the result
 * @return 0 on success, -1 on error
 */
static int str_to_bool(const char *value, bool *result)
{
    char *lowercase = strdup(value);
    if (!lowercase)
        return -1;

    // Convert to lowercase for case-insensitive comparison
    for (char *p = lowercase; *p; p++)
        *p = tolower(*p);

    // Check for true values
    if (strcmp(lowercase, "true") == 0 || strcmp(lowercase, "yes") == 0 ||
        strcmp(lowercase, "1") == 0 || strcmp(lowercase, "on") == 0 ||
        strcmp(lowercase, "y") == 0) {
        *result = true;
        free(lowercase);
        return 0;
    }

    // Check for false values
    if (strcmp(lowercase, "false") == 0 || strcmp(lowercase, "no") == 0 ||
        strcmp(lowercase, "0") == 0 || strcmp(lowercase, "off") == 0 ||
        strcmp(lowercase, "n") == 0) {
        *result = false;
        free(lowercase);
        return 0;
    }

    // Not a recognized boolean value
    free(lowercase);
    return -1;
}

/**
 * Set or update a key-value pair in the map
 */
static int set_kv_pair(cargs_t *cargs, cargs_option_t *option, char *pair)
{
    // Find the separator '='
    char *separator = strchr(pair, '=');
    if (separator == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                           "Invalid key-value format, expected 'key=value': '%s'", pair);
    }

    // Split the string at the separator
    char *key = strndup(pair, separator - pair);
    if (key == NULL) {
        CARGS_INTERNAL_ERROR(cargs, "Memory allocation failed");
    }
    char *value = separator + 1;

    // Convert the string value to boolean
    bool bool_value;
    if (str_to_bool(value, &bool_value) != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                           "Invalid boolean value for key '%s': '%s' (expected true/false, yes/no, "
                           "1/0, on/off, y/n)",
                           key, value);
    }

    // Check if the key already exists
    int key_index = map_find_key(option, key);

    // Key exists, update value
    if (key_index >= 0) {
        option->value.as_map[key_index].value.as_bool = bool_value;
    } else {
        // Key doesn't exist, add new entry
        adjust_map_size(option);

        option->value.as_map[option->value_count].key           = key;
        option->value.as_map[option->value_count].value.as_bool = bool_value;
        option->value_count++;
    }

    return CARGS_SUCCESS;
}

/**
 * Handler for boolean map options
 * Format: "key1=true,key2=false,key3=yes,key4=no,..."
 * Accepts various boolean representations: true/false, yes/no, 1/0, on/off, y/n
 * If key exists, replaces the value, otherwise creates a new entry
 */
int map_bool_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    // Process comma-separated pairs
    if (strchr(value, ',') != NULL) {
        char **pairs = split(value, ",");
        if (pairs == NULL) {
            CARGS_INTERNAL_ERROR(cargs, "Memory allocation failed");
        }

        for (size_t i = 0; pairs[i] != NULL; ++i) {
            int status = set_kv_pair(cargs, option, pairs[i]);
            if (status != CARGS_SUCCESS) {
                free_split(pairs);
                return status;
            }
        }

        free_split(pairs);
    } else {
        // Single key-value pair
        int status = set_kv_pair(cargs, option, value);
        if (status != CARGS_SUCCESS)
            return status;
    }

    apply_map_flags(option);
    option->is_allocated = true;
    return CARGS_SUCCESS;
}

/**
 * Free handler for boolean map options
 */
int free_map_bool_handler(cargs_option_t *option)
{
    if (option->value.as_map != NULL) {
        // No need to free boolean values
        for (size_t i = 0; i < option->value_count; ++i)
            free((void *)option->value.as_map[i].key);
        free(option->value.as_map);
    }
    return CARGS_SUCCESS;
}
