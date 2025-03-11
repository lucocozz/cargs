/**
 * cargs/callbacks.h - Public callbacks
 *
 * This header defines custom callback types that can be implemented
 * by users to extend the library's functionality.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_CALLBACKS_H
 #define CARGS_CALLBACKS_H
 
 #include "cargs/types.h"
 
 /**
  * Custom Handler Example:
  * 
  * int my_custom_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
  * {
  *     // Process argument 'arg' as needed
  *     // Set option->value to the processed value
  *     option->value.as_int = process_arg(arg);
  *     return CARGS_SUCCESS;
  * }
  */
 
 /**
  * Custom Validator Example:
  * 
  * int my_custom_validator(cargs_t *cargs, value_t value, validator_data_t data)
  * {
  *     // Validate value based on data
  *     // Return CARGS_SUCCESS if valid, error code otherwise
  *     if (valid_condition)
  *         return CARGS_SUCCESS;
  *     return CARGS_ERROR_INVALID_VALUE;
  * }
  */
 
 #endif /* CARGS_CALLBACKS_H */
