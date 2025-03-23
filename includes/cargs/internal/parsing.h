/**
 * cargs/internal/parsing.h - Internal parsing functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines internal functions for parsing command-line arguments.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_INTERNAL_PARSING_H
#define CARGS_INTERNAL_PARSING_H

#include "cargs/types.h"

/**
 * parse_args - Parse an array of command-line arguments
 *
 * @param cargs    Cargs context
 * @param options  Options array
 * @param argc     Argument count
 * @param argv     Argument values
 *
 * @return Status code
 */
int parse_args(cargs_t *cargs, cargs_option_t *options, int argc, char **argv);

/**
 * Handle different types of arguments
 */
int handle_subcommand(cargs_t *cargs, cargs_option_t *option, int argc, char **argv);
int handle_positional(cargs_t *cargs, cargs_option_t *options, char *value, int position);
int handle_long_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc,
                       int *current_index);
int handle_short_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc,
                        int *current_index);

/**
 * Validation and callback execution
 */
int post_parse_validation(cargs_t *cargs);
int execute_callbacks(cargs_t *cargs, cargs_option_t *option, char *value);

/**
 * Load option values from environment variables
 */
int load_env_vars(cargs_t *cargs);

#endif /* CARGS_INTERNAL_PARSING_H */
