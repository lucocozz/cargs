#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


cargs_status_t bool_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
	UNUSED(cargs);
	UNUSED(arg);
	option->value.as_bool = !option->value.as_bool;
	return (cargs_msg_success());
}
