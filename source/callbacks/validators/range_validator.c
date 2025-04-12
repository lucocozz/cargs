#include "cargs/errors.h"
#include "cargs/types.h"

int range_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    if (data.range.min > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
        return (CARGS_ERROR_INVALID_RANGE);
    }

    if (option->value.as_int < data.range.min || option->value.as_int > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Value %d is out of range [%ld, %ld]",
                           option->value.as_int, data.range.min, data.range.max);
    }
    return (CARGS_SUCCESS);
}
