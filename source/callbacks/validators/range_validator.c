#include "cargs/errors.h"
#include "cargs/types.h"

int range_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    if (value.as_int < data.range.min || value.as_int > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Value %d is out of range [%ld, %ld]",
                           value.as_int, data.range.min, data.range.max);
    }
    return (CARGS_SUCCESS);
}
