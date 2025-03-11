/**
 * cargs/internal/callbacks/validators.h - Internal validators implementation
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines standard validator callbacks for different validation types.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

 #ifndef CARGS_INTERNAL_CALLBACKS_VALIDATORS_H
 #define CARGS_INTERNAL_CALLBACKS_VALIDATORS_H
 
 #include "cargs/types.h"
 
 /**
  * range_validator - Validate that a numeric value is within a specified range
  *
  * @param cargs  Cargs context
  * @param value  Value to validate
  * @param data   Validator data containing range information
  *
  * @return Status code (0 for success, non-zero for error)
  */
 int range_validator(cargs_t *cargs, value_t value, validator_data_t data);
 
 /**
  * Other validators can be added here
  */
 
 #endif /* CARGS_INTERNAL_CALLBACKS_VALIDATORS_H */
