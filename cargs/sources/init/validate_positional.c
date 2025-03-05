#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_positional_validity(cargs_t *cargs, cargs_option_t *option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);

	if (option->name == NULL) {
		status.code = CARGS_ERROR_MALFORMED_OPTION;
		status.message = "Positional option must have a name";
		cargs_push_error(cargs, status);
	}

	if (option->flags & ~POSITIONAL_FLAG_MASK) {
		status.code = CARGS_ERROR_INVALID_FLAG;
		cargs_push_error(cargs, status);
	}

	if (option->choices_count > 0 && option->value.raw != 0)
	{
		bool valid_default = false;
		for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_default) {
			status.code = CARGS_ERROR_INVALID_DEFAULT;
			status.message = "Default value must be one of the available choices";
			cargs_push_error(cargs, status);
		}
	}

	return (status);
}
