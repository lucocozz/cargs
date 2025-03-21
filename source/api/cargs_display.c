/**
 * cargs_display.c - Display functions for cargs
 *
 * This file implements functions for displaying help, usage, and version information.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#include "cargs/internal/display.h"
#include "cargs/types.h"
#include <stdio.h>

/**
 * cargs_print_help - Print help message for command-line options
 *
 * param cargs  Cargs context
 */
void cargs_print_help(cargs_t cargs)
{
    display_help(&cargs, NULL);
}

/**
 * cargs_print_usage - Print short usage information
 *
 * param cargs  Cargs context
 */
void cargs_print_usage(cargs_t cargs)
{
    display_usage(&cargs, NULL);
}

/**
 * cargs_print_version - Print version information
 *
 * param cargs  Cargs context
 */
void cargs_print_version(cargs_t cargs)
{
    display_version(&cargs);
}
