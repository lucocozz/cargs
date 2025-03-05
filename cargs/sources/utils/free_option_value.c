#include "cargs/types.h"

#include <stdlib.h>


void	free_option_value(cargs_option_t *option)
{
	if (option->is_allocated == false)
		return;

	if (option->free_handler != NULL)
		option->free_handler(option);
	else
		free(option->value.as_ptr);
}
