/**
 * cargs/internal/display.h - Internal display functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines internal functions for displaying help, usage, and version info.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_INTERNAL_DISPLAY_H
 #define CARGS_INTERNAL_DISPLAY_H
 
 #include "cargs/types.h"
 
 /*
  * Configuration constants for display formatting
  */
 #define DESCRIPTION_COLUMN 32     // Column where descriptions start
 #define MAX_LINE_WIDTH 80         // Maximum width of a line
 #define OPTION_INDENT 2           // Indentation for options
 
 /**
  * display_help - Display help information
  *
  * @param cargs  Cargs context
  * @param cmd    Specific subcommand to display help for, or NULL for general help
  */
 void display_help(cargs_t *cargs, const cargs_option_t *cmd);
 
 /**
  * display_usage - Display short usage information
  *
  * @param cargs  Cargs context
  * @param cmd    Specific subcommand to display usage for, or NULL for general usage
  */
 void display_usage(cargs_t *cargs, const cargs_option_t *cmd);
 
 /**
  * display_version - Display version information
  *
  * @param cargs  Cargs context
  */
 void display_version(cargs_t *cargs);
 
 #endif /* CARGS_INTERNAL_DISPLAY_H */
