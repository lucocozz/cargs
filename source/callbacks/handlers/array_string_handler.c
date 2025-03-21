#include <stdlib.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/options.h"

static void _set_value(cargs_option_t *option, char *value)
{
    adjust_array_size(option);
    option->value.as_array[option->value_count].as_string = strdup(value);
    option->value_count++;
}

int array_string_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
    UNUSED(cargs);

    if (strchr(value, ',') != NULL) {
        char **splited_values = split(value, ",");
        if (splited_values == NULL)
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MEMORY, "Failed to split string '%s'", value);
        for (size_t i = 0; splited_values[i] != NULL; ++i)
            _set_value(option, splited_values[i]);
        free(splited_values);
    } else
        _set_value(option, value);

    apply_array_flags(option);
    option->is_allocated = true;
    return (CARGS_SUCCESS);
}

int free_array_string_handler(cargs_option_t *option)
{
    for (size_t i = 0; i < option->value_count; ++i)
        free(option->value.as_array[i].as_string);
    free(option->value.as_array);
    return (CARGS_SUCCESS);
}
