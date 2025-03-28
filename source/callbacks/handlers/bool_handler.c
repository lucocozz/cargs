#include <ctype.h>
#include <stddef.h>
#include <string.h>

#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"

static int string_to_bool(const char *arg)
{
    char  value[6]       = {0};
    char *false_values[] = {"0", "false", "no", "n", "off", "0x0", "0b0"};
    char *true_values[]  = {"1", "true", "yes", "y", "on", "0x1", "0b1"};

    if (strlen(arg) > sizeof(value) - 1)
        return -1;

    size_t arg_len = strlen(arg);
    for (size_t i = 0; i < arg_len; ++i)
        value[i] = tolower(arg[i]);
    value[arg_len] = '\0';

    for (size_t i = 0; i < ARRAY_SIZE(true_values); ++i) {
        if (strcmp(value, true_values[i]) == 0)
            return true;
    }

    for (size_t i = 0; i < ARRAY_SIZE(false_values); ++i) {
        if (strcmp(value, false_values[i]) == 0)
            return false;
    }

    return -1;  // Invalid boolean value
}

int bool_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    UNUSED(cargs);
    if (arg == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT, "Invalid argument for option: '%s'",
                           option->name);
        return (CARGS_ERROR_INVALID_ARGUMENT);
    }

    int is_bool = string_to_bool(arg);
    if (is_bool == -1) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT,
                           "Invalid boolean value: '%s'. Expected 'true' or 'false'", arg);
        return (CARGS_ERROR_INVALID_ARGUMENT);
    }
    option->value.as_bool = is_bool;
    return (CARGS_SUCCESS);
}
