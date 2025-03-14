/**
* usage_display.c - Functions for displaying usage information
*
* This file implements the usage display functionality for the cargs library.
* 
* MIT License - Copyright (c) 2024 lucocozz
*/

#include <stdio.h>
#include "cargs/types.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/display.h"
#include "cargs/internal/context.h"


static bool _have_subcommand(const cargs_option_t *options)
{
	for (int i = 0; options[i].type != TYPE_NONE; ++i) {
		if (options[i].type == TYPE_SUBCOMMAND)
			return (true);
	}
	return (false);
}

void display_usage(cargs_t *cargs, const cargs_option_t *cmd)
{
	UNUSED(cmd);
	const cargs_option_t *options = get_active_options(cargs);

	printf("Usage: %s", cargs->program_name);

	// Add subcommand chain if any
	for (size_t i = 0; i < cargs->context.subcommand_depth; ++i)
		printf(" %s", cargs->context.subcommand_stack[i]->name);

	// Add the current command if specified
	// if (cmd && cargs->context.subcommand_depth == 0)
		// printf(" %s", cmd->name);

	printf(" [OPTIONS]");

	// Add positional arguments to usage line
	for (int i = 0; options[i].type != TYPE_NONE; ++i)
	{
		if (options[i].type == TYPE_POSITIONAL) {
			if (options[i].flags & FLAG_REQUIRED)
				printf(" <%s>", options[i].name);
			else
				printf(" [%s]", options[i].name);
		}
	}
	
	// Add COMMAND placeholder if we have subcommands
	if (_have_subcommand(options))
		printf(" COMMAND");
	
	printf("\n");
}
