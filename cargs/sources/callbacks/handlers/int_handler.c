#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>
#include <stdlib.h>


cargs_status_t	int_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
	UNUSED(cargs);
	option->value = (value_t){ .as_int64 = atoll(value) };
	return (cargs_msg_success());
}
