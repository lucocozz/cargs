#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>
#include <stdlib.h>


int	int_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
	UNUSED(cargs);
	option->value = (value_t){ .as_int64 = atoll(value) };
	return (CARGS_SUCCESS);
}
