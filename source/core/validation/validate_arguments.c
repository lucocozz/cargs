#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"

static cargs_error_t __validate_basics(cargs_t *cargs, cargs_option_t *option)
{
	if (option->sname == 0 && option->lname == NULL) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
			"Option must have a short name or a long name");
	}
	if (option->flags & ~OPTION_FLAG_MASK) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG,
			"Invalid flag for option: '%s'", option->name);
	}
	if (option->handler == NULL) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER,
			"Option '%s' must have a handler", option->name);
	}

	return (CARGS_OK());
}

static cargs_error_t __validate_default_value(cargs_t *cargs, cargs_option_t *option)
{
	if (option->choices_count > 0 && option->have_default)
	{
		bool valid_default = false;
		for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_default) {
			CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEFAULT,
				"Default value of option '%s' must be one of the available choices", option->name);
		}
	}

	return (CARGS_OK());
}

static cargs_error_t __validate_dependencies(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{

	if (option->requires != NULL && option->conflicts != NULL)
	{
		for (int i = 0; option->requires[i] != NULL; ++i) {
			for (int j = 0; option->conflicts[j] != NULL; ++j)
			{
				if (strcmp(option->requires[i], option->conflicts[j]) == 0) {
					CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
						"Option '%s' cannot require and conflict with the same option: '%s'",
						option->name, option->requires[i]);
				}
			}
		}
	}

	if (option->requires != NULL)
	{
		for (int i = 0; option->requires[i] != NULL; ++i)
		{
			cargs_option_t *required = find_option_by_name(options, option->requires[i]);
			if (required == NULL) {
				CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
					"Required option not found '%s' in option '%s'", option->requires[i], option->name);
			}
		}
	}

	if (option->conflicts != NULL)
	{
		for (int i = 0; option->conflicts[i] != NULL; ++i)
		{
			cargs_option_t *conflict = find_option_by_name(options, option->conflicts[i]);
			if (conflict == NULL) {
				CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEPENDENCY,
					"Conflicting option not found '%s' in option '%s'", option->conflicts[i], option->name);
			}
		}
	}

	return (CARGS_OK());
}

cargs_error_t validate_option(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
	cargs_error_t status = CARGS_OK();

	status = __validate_basics(cargs, option);
	if (status.code != CARGS_SUCCESS)
		return (status);

	status = __validate_default_value(cargs, option);
	if (status.code != CARGS_SUCCESS)
		return (status);

	status = __validate_dependencies(cargs, options, option);
	return (status);
}

cargs_error_t validate_positional(cargs_t *cargs, cargs_option_t *option)
{
	if (option->name == NULL) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
			"Positional option must have a name");
	}

	if (option->flags & ~POSITIONAL_FLAG_MASK) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG,
			"Invalid flags for positional option '%s'", option->name);
	}

	if (option->choices_count > 0 && option->value.raw != 0)
	{
		bool valid_default = false;
		for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_default) {
			CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_DEFAULT,
				"Default value of positional option '%s' must be one of the available choices", option->name);
		}
	}

	return (CARGS_OK());
}
