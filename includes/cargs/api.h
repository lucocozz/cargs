/**
 * cargs/api.h - Public API functions
 *
 * This header defines all the public functions that users can call
 * to initialize, parse arguments, and access results.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_API_H
#define CARGS_API_H

#include "cargs/types.h"

/**
 * cargs_init - Initialize the cargs context
 *
 * @param options      Array of command-line options
 * @param program_name Name of the program
 * @param version      Version string
 * @param description  Program description
 *
 * @return Initialized cargs_t context
 */
cargs_t cargs_init(cargs_option_t *options, const char *program_name, 
				   const char *version, const char *description);

/**
 * cargs_parse - Parse command-line arguments
 *
 * @param cargs  Cargs context
 * @param argc   Argument count (from main)
 * @param argv   Argument values (from main)
 *
 * @return Status code (0 for success, non-zero for error)
 */
int     cargs_parse(cargs_t *cargs, int argc, char **argv);

/**
 * cargs_free - Clean up and free resources
 *
 * @param cargs  Cargs context
 */
void    cargs_free(cargs_t *cargs);

/**
 * Display functions
 */
void    cargs_print_help(cargs_t cargs);
void    cargs_print_usage(cargs_t cargs);
void    cargs_print_version(cargs_t cargs);

/**
 * cargs_is_set - Check if an option was set on the command line
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return true if the option was set, false otherwise
 */
bool    cargs_is_set(cargs_t cargs, const char *option_path);

/**
 * cargs_get_value - Get the value of an option
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Value of the option, or {0} if not found
 */
value_t cargs_get_value(cargs_t cargs, const char *option_path);


/**
 * cargs_have_subcommand - Check if a subcommand was parsed
 *
 * @param cargs  Cargs context
 *
 * @return true if a subcommand was parsed, false otherwise
 */
bool cargs_have_subcommand(cargs_t cargs);


/**
 * cargs_execute_command - Execute the parsed subcommand
 *
 * @param cargs  Cargs context
 * @param data   Data to pass to the subcommand action
 *
 * @return Status code (0 for success, non-zero for error)
 */
int	cargs_execute_command(cargs_t *cargs, void *data);


#endif /* CARGS_API_H */
