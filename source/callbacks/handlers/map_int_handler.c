#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/options.h"

/**
 * Set or update a key-value pair in the map
 */
static int _set_kv_pair(cargs_t *cargs, cargs_option_t *option, char *pair)
{
    // Find the separator '='
    char *separator = strchr(pair, '=');
    if (separator == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                           "Invalid key-value format, expected 'key=value': '%s'", pair);
    }

    // Split the string at the separator
    *separator  = '\0';
    char *key   = pair;
    char *value = separator + 1;

    // Convert the string value to integer
    char   *endptr;
    int64_t int_value = strtoll(value, &endptr, 10);

    // Check if conversion was successful
    if (*value == '\0' || *endptr != '\0') {
        // Restore the separator for error reporting purposes
        *separator = '=';
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                           "Invalid integer value for key '%s': '%s'", key, value);
    }

    // Check if the key already exists
    int key_index = map_find_key(option, key);

    // Key exists, update value
    if (key_index >= 0)
        option->value.as_map[key_index].value.as_int64 = int_value;
    else {
        // Key doesn't exist, add new entry
        adjust_map_size(option);

        // Allocate and store key
        option->value.as_map[option->value_count].key = strdup(key);
        if (option->value.as_map[option->value_count].key == NULL) {
            // Restore the separator for error reporting
            *separator = '=';
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MEMORY, "Failed to allocate memory for key '%s'",
                               key);
        }

        // Store integer value
        option->value.as_map[option->value_count].value.as_int64 = int_value;
        option->value_count++;
    }

    // Restore the separator for error reporting purposes
    *separator = '=';

    return CARGS_SUCCESS;
}

/**
 * Handler for integer map options
 * Format: "key1=42,key2=100,..."
 * If key exists, replaces the value, otherwise creates a new entry
 */
int map_int_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    // Process comma-separated pairs
    if (strchr(value, ',') != NULL) {
        char **pairs = split(value, ",");
        if (pairs == NULL) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MEMORY, "Failed to split string '%s'", value);
        }

        for (size_t i = 0; pairs[i] != NULL; ++i) {
            int status = _set_kv_pair(cargs, option, pairs[i]);
            if (status != CARGS_SUCCESS) {
                free_split(pairs);
                return status;
            }
        }

        free_split(pairs);
    } else {
        // Single key-value pair
        int status = _set_kv_pair(cargs, option, value);
        if (status != CARGS_SUCCESS)
            return status;
    }

    apply_map_flags(option);
    option->is_allocated = true;
    return CARGS_SUCCESS;
}

/**
 * Free handler for integer map options
 */
int free_map_int_handler(cargs_option_t *option)
{
    if (option->value.as_map != NULL) {
        // No need to free integer values
        for (size_t i = 0; i < option->value_count; ++i)
            free((void *)option->value.as_map[i].key);
        free(option->value.as_map);
    }
    return CARGS_SUCCESS;
}
