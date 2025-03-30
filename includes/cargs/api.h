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

cargs_t cargs_init_mode(cargs_option_t *options, const char *program_name, const char *version,
                        bool release_mode);

#ifdef CARGS_RELEASE
/**
 * cargs_init - Initialize the cargs context
 *
 * @param options      Array of command-line options
 * @param program_name Name of the program
 * @param version      Version string
 * @param description  Program description
 *
 * @return Initialized cargs_t context
 *
 * @note
 * Define `CARGS_RELEASE` when compiling your application to skip
 * options structure validation and improve performance.
 *
 * Example: `gcc -DCARGS_RELEASE my_program.c -o my_program -lcargs`
 *
 * Note: Only use this in production. During development, leave validation
 * enabled to catch configuration errors early.
 */
cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version)
{
    return cargs_init_mode(options, program_name, version, true);
}
#else
/**
 * cargs_init - Initialize the cargs context
 *
 * @param options      Array of command-line options
 * @param program_name Name of the program
 * @param version      Version string
 * @param description  Program description
 *
 * @return Initialized cargs_t context
 *
 * @note
 * Define `CARGS_RELEASE` when compiling your application to skip
 * options structure validation and improve performance.
 *
 * Example: `gcc -DCARGS_RELEASE my_program.c -o my_program -lcargs`
 *
 * Note: Only use this in production. During development, leave validation
 * enabled to catch configuration errors early.
 */
cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version)
{
    return cargs_init_mode(options, program_name, version, false);
}
#endif

/**
 * cargs_parse - Parse command-line arguments
 *
 * @param cargs  Cargs context
 * @param argc   Argument count (from main)
 * @param argv   Argument values (from main)
 *
 * @return Status code (0 for success, non-zero for error)
 */
int cargs_parse(cargs_t *cargs, int argc, char **argv);

/**
 * cargs_free - Clean up and free resources
 *
 * @param cargs  Cargs context
 */
void cargs_free(cargs_t *cargs);

/**
 * Display functions
 */
void cargs_print_help(cargs_t cargs);
void cargs_print_usage(cargs_t cargs);
void cargs_print_version(cargs_t cargs);

/**
 * cargs_is_set - Check if an option was set on the command line
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return true if the option was set, false otherwise
 */
bool cargs_is_set(cargs_t cargs, const char *option_path);

/**
 * cargs_get - Get the value of an option
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Value of the option, or {0} if not found
 */
cargs_value_t cargs_get(cargs_t cargs, const char *option_path);

/**
 * cargs_count - Get the number of values for an option
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Number of values for the option
 */
size_t cargs_count(cargs_t cargs, const char *option_path);

/**
 * cargs_has_command - Check if a subcommand was parsed
 *
 * @param cargs  Cargs context
 *
 * @return true if a subcommand was parsed, false otherwise
 */
bool cargs_has_command(cargs_t cargs);

/**
 * cargs_exec - Execute the parsed subcommand
 *
 * @param cargs  Cargs context
 * @param data   Data to pass to the subcommand action
 *
 * @return Status code (0 for success, non-zero for error)
 */
int cargs_exec(cargs_t *cargs, void *data);

/**
 * cargs_array_get - Get an element from an array option at the specified index
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 * @param index        Index of the element to retrieve
 *
 * @return Value of the element at the specified index, or {0} if not found or index out of bounds
 */
cargs_value_t cargs_array_get(cargs_t cargs, const char *option_path, size_t index);

/**
 * cargs_map_get - Get a value from a map option with the specified key
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 * @param key          Key to look up in the map
 *
 * @return Value associated with the key, or {0} if not found
 */
cargs_value_t cargs_map_get(cargs_t cargs, const char *option_path, const char *key);

/**
 * cargs_array_it - Create an iterator for efficiently traversing an array option
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Iterator structure for the array, with count=0 if option not found
 */
cargs_array_it_t cargs_array_it(cargs_t cargs, const char *option_path);

/**
 * cargs_array_next - Get the next element from an array iterator
 *
 * @param it      Array iterator
 *
 * @return true if a value was retrieved, false if end of array
 */
bool cargs_array_next(cargs_array_it_t *it);

/**
 * cargs_array_reset - Reset an array iterator to the beginning
 *
 * @param it  Array iterator to reset
 */
void cargs_array_reset(cargs_array_it_t *it);

/**
 * cargs_map_it - Create an iterator for efficiently traversing a map option
 *
 * @param cargs        Cargs context
 * @param option_path  Option path (name or subcommand.name format)
 *
 * @return Iterator structure for the map, with count=0 if option not found
 */
cargs_map_it_t cargs_map_it(cargs_t cargs, const char *option_path);

/**
 * cargs_map_next - Get the next key-value pair from a map iterator
 *
 * @param it    Map iterator
 *
 * @return true if a pair was retrieved, false if end of map
 */
bool cargs_map_next(cargs_map_it_t *it);

/**
 * cargs_map_reset - Reset a map iterator to the beginning
 *
 * @param it  Map iterator to reset
 */
void cargs_map_reset(cargs_map_it_t *it);

#endif /* CARGS_API_H */
