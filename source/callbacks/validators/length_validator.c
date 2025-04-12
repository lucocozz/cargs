#include "cargs/errors.h"
#include "cargs/types.h"

#include <string.h>

int length_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    if (option->value.as_string == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Value is NULL");
        return (CARGS_ERROR_INVALID_VALUE);
    }
    if (data.range.min < 0 || data.range.max < 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Range is negative");
        return (CARGS_ERROR_INVALID_RANGE);
    }
    if (data.range.min > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
        return (CARGS_ERROR_INVALID_RANGE);
    }

    long long len = strlen(option->value.as_string);

    if (len < data.range.min || len > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Value %d is out of length [%ld, %ld]",
                           len, data.range.min, data.range.max);
    }
    return (CARGS_SUCCESS);
}
