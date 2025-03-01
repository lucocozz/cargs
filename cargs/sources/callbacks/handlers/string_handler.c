#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


void	string_handler(cargs_option_t *option, char *value)
{
	option->value = (value_t){ .as_string = value };
	if (option->validator != NULL)
		option->validator(option->value, option->validator_data);
	option->is_set = true;
}
