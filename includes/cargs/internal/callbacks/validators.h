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
int range_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);

/**
 * regex_validator - Validate that a string value matches a regex pattern
 *
 * @param cargs  Cargs context
 * @param value  Value to validate
 * @param data   Validator data containing regex information
 *
 * @return Status code (0 for success, non-zero for error)
 */
int regex_validator(cargs_t *cargs, const char *value, validator_data_t data);

#endif /* CARGS_INTERNAL_CALLBACKS_VALIDATORS_H */
