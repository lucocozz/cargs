#ifndef CARGS_API_H
#define CARGS_API_H

#include "cargs/types.h"

cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version, const char *description);
int		cargs_parse(cargs_t *cargs, int argc, char **argv);
void	cargs_free(cargs_t *cargs);

void cargs_print_help(cargs_t cargs);
void cargs_print_usage(cargs_t cargs);
void cargs_print_version(cargs_t cargs);

bool	cargs_is_set(cargs_t cargs, const char *option_path);
value_t	cargs_get_value(cargs_t cargs, const char *option_path);

#endif // CARGS_API_H
