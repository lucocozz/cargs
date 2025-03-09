#ifndef CARGS_DEBUG_H
#define CARGS_DEBUG_H

#include "cargs/types.h"

void	print_option(cargs_option_t *option);
void	print_subcommand(cargs_option_t *subcommand);
void	print_group(cargs_option_t *group);
void	print_positional(cargs_option_t *positional);
void	print_options(cargs_option_t *options);


#endif // CARGS_DEBUG_H
