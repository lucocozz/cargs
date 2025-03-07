#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


cargs_status_t	string_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
	UNUSED(cargs);
	option->value = (value_t){ .as_string = value };
	return (cargs_msg_success());
}
