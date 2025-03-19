// tests/unit/test_callbacks/test_validators.c
#include <criterion/criterion.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"

// Forward declarations of validators to test
int range_validator(cargs_t *cargs, value_t value, validator_data_t data);
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
    value_t val1 = {.as_int = 1};
    value_t val2 = {.as_int = 50};
    value_t val3 = {.as_int = 100};
    
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
    value_t val1 = {.as_int = 0};    // Below min
    value_t val2 = {.as_int = 101};  // Above max
    
    cr_assert_neq(range_validator(&test_cargs, val1, data), CARGS_SUCCESS, "Value below min should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value below min");
    
    // Reset error stack
    test_cargs.error_stack.count = 0;
    
    cr_assert_neq(range_validator(&test_cargs, val2, data), CARGS_SUCCESS, "Value above max should fail");
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error should be reported for value above max");
}

// Note: Testing regex_validator thoroughly would require mocking PCRE2 functionality,
// which is beyond the scope of this test. A basic test could be:

Test(validators, regex_validator_basic)
{
    // This is a placeholder test that would need more setup
    cr_assert(true, "Placeholder for regex validator tests");
}
