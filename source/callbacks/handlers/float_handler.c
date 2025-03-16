#include <stdlib.h>

#include "cargs/options.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"


int  float_handler(cargs_t *cargs, cargs_option_t *option, char *value)
{
	UNUSED(cargs);
	option->value.as_float = atof(value);
	return (CARGS_SUCCESS);
}

