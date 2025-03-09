#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_positional_validity(cargs_t *cargs, cargs_option_t *option)
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
