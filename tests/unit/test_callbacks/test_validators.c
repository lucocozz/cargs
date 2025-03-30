#include <criterion/criterion.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/callbacks/validators.h"

// Forward declarations of validators to test
int range_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data);
int regex_validator(cargs_t *cargs, const char *value, validator_data_t data);

// Mock cargs context for testing
static cargs_t test_cargs;

// Setup function
void setup(void)
{
    test_cargs.program_name = "test_prog";
    test_cargs.error_stack.count = 0;
}

Test(validators, range_validator_valid, .init = setup)
{
    // Setup range validation for 1-100
    validator_data_t data = {.range = {.min = 1, .max = 100}};
    
    // Valid cases
    cargs_value_t val1 = {.as_int = 1};
    cargs_value_t val2 = {.as_int = 50};
    cargs_value_t val3 = {.as_int = 100};
    
    cr_assert_eq(range_validator(&test_cargs, val1, data), CARGS_SUCCESS, "Min value should be valid");
    cr_assert_eq(range_validator(&test_cargs, val2, data), CARGS_SUCCESS, "Middle value should be valid");
    cr_assert_eq(range_validator(&test_cargs, val3, data), CARGS_SUCCESS, "Max value should be valid");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported for valid values");
}

Test(validators, range_validator_invalid, .init = setup)
{
    // Setup range validation for 1-100
    validator_data_t data = {.range = {.min = 1, .max = 100}};
    
    // Invalid cases
    cargs_value_t val1 = {.as_int = 0};    // Below min
    cargs_value_t val2 = {.as_int = 101};  // Above max
    
    cr_assert_neq(range_validator(&test_cargs, val1, data), CARGS_SUCCESS, "Value below min should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value below min");
    
    // Reset error stack
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(range_validator(&test_cargs, val2, data), CARGS_SUCCESS, "Value above max should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value above max");
}

Test(validators, range_validator_equal_bounds, .init = setup)
{
    // Setup range validation with equal min and max
    validator_data_t data = {.range = {.min = 42, .max = 42}};
    
    // Valid case - matching the only valid value
    cargs_value_t val1 = {.as_int = 42};
    cr_assert_eq(range_validator(&test_cargs, val1, data), CARGS_SUCCESS, "Equal bounds value should be valid");
    
    // Invalid cases
    cargs_value_t val2 = {.as_int = 41};
    cargs_value_t val3 = {.as_int = 43};
    
    cr_assert_neq(range_validator(&test_cargs, val2, data), CARGS_SUCCESS, "Value below equal bounds should fail");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(range_validator(&test_cargs, val3, data), CARGS_SUCCESS, "Value above equal bounds should fail");
}

Test(validators, range_validator_negative_values, .init = setup)
{
    // Setup range validation with negative values
    validator_data_t data = {.range = {.min = -100, .max = -1}};
    
    // Valid cases
    cargs_value_t val1 = {.as_int = -100};
    cargs_value_t val2 = {.as_int = -50};
    cargs_value_t val3 = {.as_int = -1};
    
    cr_assert_eq(range_validator(&test_cargs, val1, data), CARGS_SUCCESS, "Negative min should be valid");
    cr_assert_eq(range_validator(&test_cargs, val2, data), CARGS_SUCCESS, "Middle negative value should be valid");
    cr_assert_eq(range_validator(&test_cargs, val3, data), CARGS_SUCCESS, "Negative max should be valid");
    
    // Invalid cases
    cargs_value_t val4 = {.as_int = -101};
    cargs_value_t val5 = {.as_int = 0};
    
    cr_assert_neq(range_validator(&test_cargs, val4, data), CARGS_SUCCESS, "Value below negative min should fail");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(range_validator(&test_cargs, val5, data), CARGS_SUCCESS, "Value above negative max should fail");
}

// Basic tests for regex_validator
Test(validators, regex_validator_basic, .init = setup)
{
    // Test with a simple pattern matching digits
    regex_data_t data = {
        .pattern = "^\\d+$",
        .hint = "Value must be digits only"
    };
    
    // Valid cases
    cr_assert_eq(regex_validator(&test_cargs, "123", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                "Digits should match digit pattern");
    cr_assert_eq(regex_validator(&test_cargs, "0", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                "Single digit should match digit pattern");
    
    // Invalid cases
    cr_assert_neq(regex_validator(&test_cargs, "abc", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "Letters should not match digit pattern");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(regex_validator(&test_cargs, "123abc", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "Mixed content should not match digit pattern");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(regex_validator(&test_cargs, "", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "Empty string should not match digit pattern");
}

Test(validators, regex_validator_email, .init = setup)
{
    // Test with an email pattern
    regex_data_t data = {
        .pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$",
        .hint = "Value must be a valid email address"
    };
    
    // Valid cases
    cr_assert_eq(regex_validator(&test_cargs, "test@example.com", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                "Valid email should match pattern");
    cr_assert_eq(regex_validator(&test_cargs, "user.name+tag@example.co.uk", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                "Complex valid email should match pattern");
    
    // Invalid cases
    cr_assert_neq(regex_validator(&test_cargs, "test", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "String without @ should not match email pattern");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(regex_validator(&test_cargs, "test@", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "Partial email should not match pattern");
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(regex_validator(&test_cargs, "test@example", (validator_data_t){.regex = data}), CARGS_SUCCESS,
                 "Email without domain extension should not match pattern");
}

Test(validators, regex_validator_null_cases, .init = setup)
{
    // Test with NULL pattern
    regex_data_t null_pattern = {
        .pattern = NULL,
        .hint = "Pattern is NULL"
    };
    
    cr_assert_neq(regex_validator(&test_cargs, "test", (validator_data_t){.regex = null_pattern}), CARGS_SUCCESS,
                 "NULL pattern should fail");
    test_cargs.error_stack.count = 0;
    
    // Valid pattern with NULL value
    regex_data_t valid_pattern = {
        .pattern = "^\\w+$",
        .hint = "Word characters only"
    };
    
    cr_assert_neq(regex_validator(&test_cargs, NULL, (validator_data_t){.regex = valid_pattern}), CARGS_SUCCESS,
                 "NULL value should fail");
}
