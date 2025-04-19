#include <stddef.h>
#ifndef CARGS_NO_REGEX
    #define PCRE2_CODE_UNIT_WIDTH 8
    #include <pcre2.h>
#endif
#include "cargs/errors.h"
#include "cargs/types.h"
#include "cargs/internal/utils.h"

/**
 * regex_validator - Validate a string value against a regular expression
 *
 * @param cargs  Cargs context
 * @param value  String value to validate
 * @param data   Validator data containing regex pattern
 *
 * @return Status code (0 for success, non-zero for error)
 */
int regex_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
#ifdef CARGS_NO_REGEX
    // Regex support is disabled
    UNUSED(value);
    UNUSED(data);
    CARGS_REPORT_ERROR(
        cargs, CARGS_ERROR_INVALID_VALUE,
        "Regex validation is not supported in this build (compiled with CARGS_NO_REGEX)");
    return CARGS_ERROR_INVALID_VALUE;
#else
    const char *pattern = data.regex.pattern;
    if (!pattern) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Regular expression pattern is NULL");
    }

    // Compile the regular expression
    int         errorcode;
    PCRE2_SIZE  erroroffset;
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
                                   &erroroffset, NULL);

    if (re == NULL) {
        // Failed to compile the regex
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT, "Failed to compile regex '%s': %s",
                           pattern, buffer);
    }

    // Execute the regex against the input string
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)value, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);

    // Free resources
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                if (data.regex.hint && data.regex.hint[0] != '\0') {
                    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Invalid value '%s': %s",
                                       value, data.regex.hint);
                } else {
                    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                       "Value '%s' does not match the expected format", value);
                }
            default:
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                                   "Internal error: Regex match failed with error code %d", rc);
        }
    }
    return (CARGS_SUCCESS);
#endif
}
