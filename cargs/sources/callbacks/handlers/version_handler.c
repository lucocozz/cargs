#include "cargs/utils.h"
#include "cargs/types.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void	cargs_print_version(cargs_t *cargs)
{
	printf("%s v%s\n", cargs->program_name, cargs->version);
	if (cargs->description != NULL)
		printf("\n%s\n", cargs->description);
}

int version_handler(cargs_t *cargs, cargs_option_t *options, char *arg)
{
	UNUSED(arg);
	cargs_print_version(cargs);
	if (options->flags & FLAG_EXIT)
		exit(EXIT_SUCCESS);
	return (CARGS_SUCCESS);
}
