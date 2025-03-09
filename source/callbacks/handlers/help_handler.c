#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


// void cargs_print_help(cargs_t *cargs)
// {
// 	cargs_help_handler(cargs, NULL, NULL);
// }

int	help_handler(cargs_t *cargs, cargs_option_t *options, char *arg)
{
	UNUSED(cargs);
	(void)options;
	UNUSED(arg);
	printf("help handler\n");
	return (CARGS_SUCCESS);
}
