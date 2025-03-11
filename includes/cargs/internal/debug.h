/**
 * cargs/internal/debug.h - Internal debugging API
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for debugging the library's internal state.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_INTERNAL_DEBUG_H
 #define CARGS_INTERNAL_DEBUG_H
 
 #include "cargs/types.h"
 
 /**
  * Debug print functions - for internal use only
  */
 void print_option(cargs_option_t *option);
 void print_subcommand(cargs_option_t *subcommand);
 void print_group(cargs_option_t *group);
 void print_positional(cargs_option_t *positional);
 void print_options(cargs_option_t *options);
 
 #endif /* CARGS_INTERNAL_DEBUG_H */
