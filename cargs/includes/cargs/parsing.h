#ifndef CARGS_PARSING_H
#define CARGS_PARSING_H

#include "cargs/types.h"

int parse_args(cargs_t *cargs, cargs_option_t *options, int argc, char **argv);
int handle_subcommand(cargs_t *cargs, cargs_option_t *option, int argc, char **argv);
int validate_dependencies(cargs_t *cargs, cargs_option_t *options);
int handle_positional(cargs_t *cargs, cargs_option_t *options, char *value, int position);
int handle_long_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc, int *current_index);
int handle_short_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc, int *current_index);


#endif // CARGS_PARSING_H
