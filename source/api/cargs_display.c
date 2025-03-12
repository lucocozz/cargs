/**
 * cargs_display.c - Display functions for cargs
 *
 * This file implements functions for displaying help, usage, and version information.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

#include <stdio.h>
#include "cargs/types.h"

/**
 * cargs_print_help - Print help message for command-line options
 *
 * @param cargs  Cargs context
 */
void cargs_print_help(cargs_t cargs)
{
    printf("Usage: %s [options]\n\n", cargs.program_name);
    
    if (cargs.description)
        printf("%s\n\n", cargs.description);
    
    printf("Options:\n");
    
    // TODO: Implement full help display
    // This is a placeholder implementation

    printf("  -h, --help     Show this help message\n");
    printf("  -V, --version  Show version information\n");
}

/**
 * cargs_print_usage - Print short usage information
 *
 * @param cargs  Cargs context
 */
void cargs_print_usage(cargs_t cargs)
{
    printf("Usage: %s [options]\n", cargs.program_name);
    printf("Try '%s --help' for more information.\n", cargs.program_name);
}

/**
 * cargs_print_version - Print version information
 *
 * @param cargs  Cargs context
 */
// void cargs_print_version(cargs_t cargs)
// {
//     printf("%s v%s\n", cargs.program_name, cargs.version);
//     if (cargs.description != NULL)
//         printf("\n%s\n", cargs.description);
// }
