#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/errors.h"


int	default_free(cargs_option_t *option)
{
	free(option->value.as_ptr);
	return (CARGS_SUCCESS);
}
