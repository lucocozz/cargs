#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/utils.h"


static void	__free_options(cargs_option_t *options)
{
	for (cargs_option_t *option = options; option->type != TYPE_NONE; ++option)
		free_option_value(option);
}

void	cargs_free(cargs_t *cargs)
{
	__free_options(cargs->options);
	for (size_t i = 0; i < cargs->subcommand_depth; ++i) {
		cargs_option_t *options = cargs->subcommand_stack[i]->subcommand.options;
		__free_options(options);
	}
}
