#include <criterion/criterion.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/callbacks/validators.h"

// Forward declarations of validators to test
int range_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int regex_validator(cargs_t *cargs, const char *value, validator_data_t data);
int length_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int count_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);

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
    cargs_option_t  option;
    
    // Valid cases
    cargs_value_t val1 = {.as_int = 1};
    cargs_value_t val2 = {.as_int = 50};
    cargs_value_t val3 = {.as_int = 100};

    option.value = val1;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Min value should be valid");
    option.value = val2;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Middle value should be valid");
    option.value = val3;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Max value should be valid");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported for valid values");
}

Test(validators, range_validator_invalid, .init = setup)
{
    // Setup range validation for 1-100
    validator_data_t data = {.range = {.min = 1, .max = 100}};
    cargs_option_t option;
    
    // Invalid cases
    cargs_value_t val1 = {.as_int = 0};    // Below min
    cargs_value_t val2 = {.as_int = 101};  // Above max
    
    option.value = val1;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value below min should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value below min");
    
    // Reset error stack
    test_cargs.error_stack.count = 0;
    
    option.value = val2;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value above max should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value above max");
}

Test(validators, range_validator_equal_bounds, .init = setup)
{
    // Setup range validation with equal min and max
    validator_data_t data = {.range = {.min = 42, .max = 42}};
    cargs_option_t option;
    
    // Valid case - matching the only valid value
    cargs_value_t val1 = {.as_int = 42};
    option.value = val1;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Equal bounds value should be valid");
    
    // Invalid cases
    cargs_value_t val2 = {.as_int = 41};
    cargs_value_t val3 = {.as_int = 43};
    
    option.value = val2;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value below equal bounds should fail");
    test_cargs.error_stack.count = 0;
    
    option.value = val3;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value above equal bounds should fail");
}

Test(validators, range_validator_negative_values, .init = setup)
{
    // Setup range validation with negative values
    validator_data_t data = {.range = {.min = -100, .max = -1}};
    cargs_option_t option;
    
    // Valid cases
    cargs_value_t val1 = {.as_int = -100};
    cargs_value_t val2 = {.as_int = -50};
    cargs_value_t val3 = {.as_int = -1};
    
    option.value = val1;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Negative min should be valid");
    
    option.value = val2;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Middle negative value should be valid");
    
    option.value = val3;
    cr_assert_eq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Negative max should be valid");
    
    // Invalid cases
    cargs_value_t val4 = {.as_int = -101};
    cargs_value_t val5 = {.as_int = 0};
    
    option.value = val4;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value below negative min should fail");
    test_cargs.error_stack.count = 0;
    
    option.value = val5;
    cr_assert_neq(range_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Value above negative max should fail");
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

// Tests for length_validator
Test(validators, length_validator_valid, .init = setup)
{
    // Setup length validation for strings between 3-10 characters
    validator_data_t data = {.range = {.min = 3, .max = 10}};
    cargs_option_t option;
    
    // Valid cases
    cargs_value_t val1 = {.as_string = "abc"};       // Min length
    cargs_value_t val2 = {.as_string = "abcdef"};    // Middle length
    cargs_value_t val3 = {.as_string = "abcdefghij"}; // Max length

    option.value = val1;
    cr_assert_eq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Min length should be valid");
    option.value = val2;
    cr_assert_eq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Middle length should be valid");
    option.value = val3;
    cr_assert_eq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Max length should be valid");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported for valid lengths");
}

Test(validators, length_validator_invalid, .init = setup)
{
    // Setup length validation for strings between 3-10 characters
    validator_data_t data = {.range = {.min = 3, .max = 10}};
    cargs_option_t option;
    
    // Invalid cases
    cargs_value_t val1 = {.as_string = "ab"};           // Below min
    cargs_value_t val2 = {.as_string = "abcdefghijk"};  // Above max
    
    option.value = val1;
    cr_assert_neq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Length below min should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for length below min");
    
    // Reset error stack
    test_cargs.error_stack.count = 0;
    
    option.value = val2;
    cr_assert_neq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Length above max should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for length above max");
}

Test(validators, length_validator_edge_cases, .init = setup)
{
    // Test with empty string
    validator_data_t data = {.range = {.min = 0, .max = 10}};
    cargs_option_t option;
    cargs_value_t val1 = {.as_string = ""};
    
    option.value = val1;
    cr_assert_eq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Empty string should be valid when min is 0");
    
    // Test with NULL string
    cargs_value_t val2 = {.as_string = NULL};
    option.value = val2;
    cr_assert_neq(length_validator(&test_cargs, &option, data), CARGS_SUCCESS, "NULL string should fail");
    test_cargs.error_stack.count = 0;
    
    // Test with exact same min and max
    validator_data_t data2 = {.range = {.min = 5, .max = 5}};
    cargs_value_t val3 = {.as_string = "12345"};
    option.value = val3;
    cr_assert_eq(length_validator(&test_cargs, &option, data2), CARGS_SUCCESS, "String with exact required length should be valid");
}

// Tests for count_validator
Test(validators, count_validator_valid, .init = setup)
{
    // Setup count validation for 1-5 values
    validator_data_t data = {.range = {.min = 1, .max = 5}};
    cargs_option_t option;
    
    // Valid cases
    option.value_count = 1;  // Min count
    cr_assert_eq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Min count should be valid");
    
    option.value_count = 3;  // Middle count
    cr_assert_eq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Middle count should be valid");
    
    option.value_count = 5;  // Max count
    cr_assert_eq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Max count should be valid");
    
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported for valid counts");
}

Test(validators, count_validator_invalid, .init = setup)
{
    // Setup count validation for 1-5 values
    validator_data_t data = {.range = {.min = 1, .max = 5}};
    cargs_option_t option;
    
    // Invalid cases
    option.value_count = 0;  // Below min
    cr_assert_neq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Count below min should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for count below min");
    
    // Reset error stack
    test_cargs.error_stack.count = 0;
    
    option.value_count = 6;  // Above max
    cr_assert_neq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Count above max should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for count above max");
}

Test(validators, count_validator_zero_allowed, .init = setup)
{
    // Setup count validation allowing zero values
    validator_data_t data = {.range = {.min = 0, .max = 3}};
    cargs_option_t option;
    
    // Test with zero count
    option.value_count = 0;
    cr_assert_eq(count_validator(&test_cargs, &option, data), CARGS_SUCCESS, "Zero count should be valid when min is 0");
    
    // Test with exact same min and max
    validator_data_t data2 = {.range = {.min = 2, .max = 2}};
    option.value_count = 2;
    cr_assert_eq(count_validator(&test_cargs, &option, data2), CARGS_SUCCESS, "Exact required count should be valid");
    
    // Invalid with exact bounds
    option.value_count = 1;
    cr_assert_neq(count_validator(&test_cargs, &option, data2), CARGS_SUCCESS, "Below exact required count should fail");
}
