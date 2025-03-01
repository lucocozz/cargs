#include "cargs/utils.h"
#include "cargs/types.h"

#include <stdio.h>


// static void	__print_param(char *sparam, char *lparam, char *help, cargs_type_t type)
// {
// 	(void)type;
// 	printf("  %4s, %s %s\n", sparam, lparam, help);
// }


// void	*cargs_help_handler(cargs_t *cargs, char *arg, void *result)
// {
	// cargs_opt_t *options = NULL;

	// (void)arg;
	// (void)result;
	// printf("Usage:\n");
	// printf("  %s\n", cargs->program_name);
	// printf("\n");

	// options = g_cargs_builtin_opts;
	// for (int i = 0; memcmp(&options[i], &(cargs_opt_t)CARGS_OPT_END, sizeof(cargs_opt_t)) != 0; ++i)
	// 	__print_param(options[i].sparam, options[i].lparam, options[i].help, options[i].arg_type);

	// options = cargs->options;
	// for (int i = 0; memcmp(&options[i], &(cargs_opt_t)CARGS_OPT_END, sizeof(cargs_opt_t)) != 0; ++i)
	// 	__print_param(options[i].sparam, options[i].lparam, options[i].help, options[i].arg_type);
	// exit(EXIT_SUCCESS);
// }

// void cargs_print_help(cargs_t *cargs)
// {
// 	cargs_help_handler(cargs, NULL, NULL);
// }

void	help_handler(cargs_option_t *options, char *arg)
{
	(void)options;
	UNUSED(arg);
	printf("help handler\n");
}
