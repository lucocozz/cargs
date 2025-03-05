#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


int	execute_callbacks(cargs_t *cargs, cargs_option_t *option, char *value)
{
	int status = CARGS_SUCCESS;

	if (option->handler == NULL)
		return (CARGS_ERROR_INVALID_HANDLER);

	status = option->handler(cargs, option, value);
	if (status != CARGS_SUCCESS)
		return (status);

	if (option->validator != NULL)
	{
		status = option->validator(option->value, option->validator_data);
		if (status != CARGS_SUCCESS) {
			free_option_value(option);
			return (status);
		}
	}

	option->is_set = true;

	return (CARGS_SUCCESS);
}
