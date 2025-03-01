#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


int bool_handler(cargs_option_t *option, char *arg)
{
	UNUSED(arg);
	option->value.as_bool = !option->value.as_bool;
	printf("bool handler");
	return (CARGS_SUCCESS);
}
