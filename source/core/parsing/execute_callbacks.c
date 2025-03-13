#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"


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

	if (option->flags & FLAG_EXIT)
		return (CARGS_SOULD_EXIT);
	return (status);
}
