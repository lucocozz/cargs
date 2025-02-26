#include "cargs/utils.h"
#include "cargs/types.h"
#include <string.h>

// void	*cargs_version_handler(cargs_t *cargs, char *arg, void *result)
// {
// 	(void)arg;
// 	(void)result;
// 	printf("%s v%s\n", cargs->program_name, cargs->program_version);
// 	exit(EXIT_SUCCESS);
// }

// void cargs_print_version(cargs_t *cargs)
// {
// 	cargs_version_handler(cargs, NULL, NULL);
// }


void version_handler(cargs_option_t *options, char *arg)
{
	(void)options;
	UNUSED(arg);
}
