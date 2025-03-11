/**
 * cargs/internal/callbacks/handlers.h - Internal handlers implementation
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines standard handler callbacks for different option types.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_INTERNAL_CALLBACKS_HANDLERS_H
 #define CARGS_INTERNAL_CALLBACKS_HANDLERS_H
 
 #include "cargs/types.h"
 
 /**
  * Type-specific option handlers
  */
 int string_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 int int_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 int bool_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 int float_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 
 /**
  * Special option handlers
  */
 int help_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 int version_handler(cargs_t *cargs, cargs_option_t *options, char *arg);
 
 #endif /* CARGS_INTERNAL_CALLBACKS_HANDLERS_H */
