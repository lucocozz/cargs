#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"


static int	_validate_choices(cargs_t *cargs, cargs_option_t *option)
{
	if (option->choices_count > 0)
	{
		bool valid_choices = false;
		for (size_t i = 0; i < option->choices_count && !valid_choices; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_choices = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_choices)
		{
			fprintf(stderr, "%s: The '%s' option cannot be set to '", cargs->program_name, option->name);
			print_value(stderr, option->value_type, option->value);
			fprintf(stderr, "'. Please choose from ");
			print_value_array(stderr, option->value_type, option->choices.as_ptr, option->choices_count);
			fprintf(stderr, "\n");
			return (CARGS_ERROR_INVALID_CHOICE);
		}
	}
	return (CARGS_SUCCESS);
}


int	execute_callbacks(cargs_t *cargs, cargs_option_t *option, char *value)
{
	int status;

	if (option->handler == NULL) {
		CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER,
			"Option %s has no handler", option->name);
	}

	status = option->handler(cargs, option, value);
	if (status != CARGS_SUCCESS)
		return (status);

	if (option->validator != NULL)
	{
		status = option->validator(cargs, option->value, option->validator_data);
		if (status != CARGS_SUCCESS) {
			free_option_value(option);
			return (status);
		}
	}

	option->is_set = true;

	status = _validate_choices(cargs, option);
	if (status != CARGS_SUCCESS)
		return (status);

	if (option->flags & FLAG_EXIT)
		return (CARGS_SOULD_EXIT);
	return (CARGS_SUCCESS);
}
