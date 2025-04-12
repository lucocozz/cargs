#include "cargs/errors.h"
#include "cargs/types.h"

int count_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    if (data.range.min < 0 || data.range.max < 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Range is negative");
        return (CARGS_ERROR_INVALID_RANGE);
    }
    if (data.range.min > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Range is invalid [%ld, %ld]",
                           data.range.min, data.range.max);
        return (CARGS_ERROR_INVALID_RANGE);
    }

    long long count = option->value_count;

    if (count < data.range.min || count > data.range.max) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE, "Values count %d is out of [%ld, %ld]",
                           count, data.range.min, data.range.max);
    }
    return (CARGS_SUCCESS);
}
