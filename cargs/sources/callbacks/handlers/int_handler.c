#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>
#include <stdlib.h>


int	int_handler(cargs_option_t *option, char *value)
{
	option->value = (value_t){ .as_int = atoll(value) };
	if (option->validator != NULL)
		option->validator(option->value, option->validator_data);
	option->is_set = true;
	return (CARGS_SUCCESS);
}
