#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/options.h"
#include "cargs/types.h"

#ifndef MIN
    #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * Helper structure to represent an integer range
 */
typedef struct
{
    int start;
    int end;
} int_range_t;

/**
 * Parse a string into an integer range
 * Formats supported: "42", "-42", "1-5", "-5-5", "-10--5"
 *
 * @param range Pointer to store the parsed range
 * @param value String to parse
 * @return 0 on success, -1 on error
 */
static int parse_int_range(int_range_t *range, const char *value)
{
    char *minus = strchr(value, '-');
    if (minus != NULL) {
        // Successfully parsed as a range
        // Normalize range using MIN/MAX
        int start    = strtol(value, NULL, 10);
        int end      = strtol(minus + 1, NULL, 10);
        range->start = MIN(start, end);
        range->end   = MAX(start, end);
        return 0;
    }

    // Not a range, try as a single value
    char *endptr = NULL;
    range->start = strtol(value, &endptr, 10);

    // Check if the entire string was a valid integer
    if (*endptr != '\0')
        return -1;  // Invalid integer format

    // Single value case (start = end)
    range->end = range->start;
    return (0);
}

/**
 * Add a range of integers to the option's value array
 */
static void add_range_values(cargs_option_t *option, const int_range_t *range)
{
    for (int i = range->start; i <= range->end; i++) {
        adjust_array_size(option);
        option->value.as_array[option->value_count].as_int = i;
        option->value_count++;
    }
}

/**
 * Process a single value or range and add it to the option
 */
static int set_value(cargs_t *cargs, cargs_option_t *option, char *value)
{
    int_range_t range;

    if (parse_int_range(&range, value) != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                           "Invalid integer or range format: '%s'", value);
    }
    add_range_values(option, &range);
    return (CARGS_SUCCESS);
}

/**
 * Handler for integer array options, supporting ranges
 * Format examples:
 *   "1,2,3,4,5"    => [1,2,3,4,5]
 *   "1-5"          => [1,2,3,4,5]
 *   "1-3,5,7-9"    => [1,2,3,5,7,8,9]
 */
int array_int_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    if (strchr(value, ',') != NULL) {
        char **splited_values = split(value, ",");
        if (splited_values == NULL) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MEMORY, "Failed to split string '%s'", value);
        }

        for (size_t i = 0; splited_values[i] != NULL; ++i) {
            int status = set_value(cargs, option, splited_values[i]);
            if (status != CARGS_SUCCESS) {
                free_split(splited_values);
                return status;
            }
        }

        free_split(splited_values);
    } else {
        int status = set_value(cargs, option, value);
        if (status != CARGS_SUCCESS)
            return status;
    }

    apply_array_flags(option);
    option->is_allocated = true;
    return (CARGS_SUCCESS);
}

/**
 * Free handler for integer array options
 */
int free_array_int_handler(cargs_option_t *option)
{
    free(option->value.as_array);
    return (CARGS_SUCCESS);
}
