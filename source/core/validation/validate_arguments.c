#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"
#include <stddef.h>
#include <string.h>

static int validate_basics(cargs_t *cargs, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    if (option->sname == 0 && option->lname == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
                            "Option must have a short name or a long name");
        status = CARGS_ERROR_MALFORMED_OPTION;
    }
    if ((option->value_type == VALUE_TYPE_PRIMITIVE && option->flags & ~OPTION_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_ARRAY && option->flags & ~OPTION_ARRAY_FLAG_MASK) ||
        (option->value_type == VALUE_TYPE_MAP && option->flags & ~OPTION_MAP_FLAG_MASK)) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG, "Invalid flag for option: '%s'",
                            option->name);
        status = CARGS_ERROR_INVALID_FLAG;
    }

    if (option->handler == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER, "Option '%s' must have a handler",
                            option->name);
        status = CARGS_ERROR_INVALID_HANDLER;
    }

    return (status);
}

static int validate_default_value(cargs_t *cargs, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    if (option->choices_count > 0 && option->have_default) {
        bool valid_default = false;
        for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
            cargs_value_t choice =
                choices_to_value(option->value_type, option->choices, option->choices_count, i);
            valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
        }
        if (!valid_default) {
            CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEFAULT,
                                "Default value of option '%s' must be one of the available choices",
                                option->name);
            status = CARGS_ERROR_INVALID_DEFAULT;
        }
    }

    return (status);
}

static int validate_dependencies(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    if (option->requires != NULL && option->conflicts != NULL) {
        for (int i = 0; option->requires[i] != NULL; ++i) {
            for (int j = 0; option->conflicts[j] != NULL; ++j) {
                if (strcmp(option->requires[i], option->conflicts[j]) == 0) {
                    CARGS_COLLECT_ERROR(
                        cargs, CARGS_ERROR_INVALID_DEPENDENCY,
                        "Option '%s' cannot require and conflict with the same option: '%s'",
                        option->name, option->requires[i]);
                    status = CARGS_ERROR_INVALID_DEPENDENCY;
                }
            }
        }
    }

    if (option->requires != NULL) {
        for (int i = 0; option->requires[i] != NULL; ++i) {
            cargs_option_t *required = find_option_by_name(options, option->requires[i]);
            if (required == NULL) {
                CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
                                    "Required option not found '%s' in option '%s'",
                                    option->requires[i], option->name);
                status = CARGS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    if (option->conflicts != NULL) {
        for (int i = 0; option->conflicts[i] != NULL; ++i) {
            cargs_option_t *conflict = find_option_by_name(options, option->conflicts[i]);
            if (conflict == NULL) {
                CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
                                    "Conflicting option not found '%s' in option '%s'",
                                    option->conflicts[i], option->name);
                status = CARGS_ERROR_INVALID_DEPENDENCY;
            }
        }
    }

    return (status);
}

int validate_option(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    status = validate_basics(cargs, option);
    if (status != CARGS_SUCCESS)
        return (status);

    status = validate_default_value(cargs, option);
    if (status != CARGS_SUCCESS)
        return (status);

    status = validate_dependencies(cargs, options, option);
    return (status);
}

int validate_positional(cargs_t *cargs, cargs_option_t *option)
{
    int status = CARGS_SUCCESS;

    if (option->name == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
                            "Positional option must have a name");
        status = CARGS_ERROR_MALFORMED_OPTION;
    }

    if (option->flags & ~POSITIONAL_FLAG_MASK) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG,
                            "Invalid flags for positional option '%s'", option->name);
        status = CARGS_ERROR_INVALID_FLAG;
    }

    if (option->handler == NULL) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER,
                            "Positional option '%s' must have a handler", option->name);
        status = CARGS_ERROR_INVALID_HANDLER;
    }

    if ((option->flags & FLAG_REQUIRED) && option->have_default) {
        CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG,
                            "Positional option '%s' cannot be required and have a default value",
                            option->name);
        status = CARGS_ERROR_INVALID_FLAG;
    }

    if (option->choices_count > 0 && option->have_default) {
        bool valid_default = false;
        for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
            cargs_value_t choice =
                choices_to_value(option->value_type, option->choices, option->choices_count, i);
            valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
        }
        if (!valid_default) {
            CARGS_COLLECT_ERROR(
                cargs, CARGS_ERROR_INVALID_DEFAULT,
                "Default value of positional option '%s' must be one of the available choices",
                option->name);
            status = CARGS_ERROR_INVALID_DEFAULT;
        }
    }

    return (status);
}
