/**
 * version_display.c - Functions for displaying version information
 *
 * This file implements the version display functionality for the cargs library.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#include "cargs/internal/display.h"
#include "cargs/types.h"
#include <stdio.h>

void display_version(cargs_t *cargs)
{
    printf("%s", cargs->program_name);

    if (cargs->version)
        printf(" v%s", cargs->version);

    printf("\n");

    if (cargs->description)
        printf("\n%s\n", cargs->description);
}
