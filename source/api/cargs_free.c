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
	for (size_t i = 0; i < cargs->context.subcommand_depth; ++i) {
		const cargs_option_t *subcommand = cargs->context.subcommand_stack[i];
		cargs_option_t *options = subcommand->subcommand.options;
		__free_options(options);
	}
}
