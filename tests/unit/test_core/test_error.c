#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/types.h"
#include <string.h>

// Test cargs context
static cargs_t test_cargs;

void setup_error(void)
{
    test_cargs.program_name = "test_program";
    test_cargs.error_stack.count = 0;
    for (size_t i = 0; i < CARGS_MAX_ERRORS_STACK; i++) {
        test_cargs.error_stack.errors[i].code = 0;
        test_cargs.error_stack.errors[i].message[0] = '\0';
        test_cargs.error_stack.errors[i].context = (cargs_error_context_t){0};
    }
}

// Test for cargs_strerror function
Test(error, cargs_strerror)
{
    // Test common error codes
    cr_assert_str_eq(cargs_strerror(CARGS_SUCCESS), "Success");
    cr_assert_str_eq(cargs_strerror(CARGS_ERROR_DUPLICATE_OPTION), "Duplicate option");
    cr_assert_str_eq(cargs_strerror(CARGS_ERROR_INVALID_ARGUMENT), "Invalid argument");
    cr_assert_str_eq(cargs_strerror(CARGS_ERROR_MISSING_REQUIRED), "Missing required option");
    
    // Test invalid error code
    cr_assert_str_eq(cargs_strerror(999), "Unknown error");
}

// Test for cargs_push_error function
Test(error, cargs_push_error, .init = setup_error)
{
    // Create a test error
    cargs_error_t error = {
        .code = CARGS_ERROR_INVALID_ARGUMENT,
        .context = {.option_name = "test_option", .group_name = NULL, .subcommand_name = NULL},
    };
    strcpy(error.message, "Test error message");
    
    // Add error to the stack
    cargs_push_error(&test_cargs, error);
    
    // Verify if error was added correctly
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error count should be 1");
    cr_assert_eq(test_cargs.error_stack.errors[0].code, CARGS_ERROR_INVALID_ARGUMENT, "Error code should match");
    cr_assert_str_eq(test_cargs.error_stack.errors[0].message, "Test error message", "Error message should match");
    cr_assert_str_eq(test_cargs.error_stack.errors[0].context.option_name, "test_option", "Option name should match");
    cr_assert_null(test_cargs.error_stack.errors[0].context.group_name, "Group name should be NULL");
    cr_assert_null(test_cargs.error_stack.errors[0].context.subcommand_name, "Subcommand name should be NULL");
}

// Test to verify behavior when error stack overflows
Test(error, cargs_push_error_overflow, .init = setup_error)
{
    // Fill the error stack
    cargs_error_t error = {
        .code = CARGS_ERROR_INVALID_ARGUMENT,
        .context = {.option_name = "option", .group_name = NULL, .subcommand_name = NULL},
    };
    strcpy(error.message, "Test error");
    
    // Add errors until stack is full
    for (size_t i = 0; i < CARGS_MAX_ERRORS_STACK; i++) {
        cargs_push_error(&test_cargs, error);
    }
    
    cr_assert_eq(test_cargs.error_stack.count, CARGS_MAX_ERRORS_STACK, "Error count should be max");
    
    // Add one more error
    error.code = CARGS_ERROR_MEMORY;
    strcpy(error.message, "Overflow error");
    cargs_push_error(&test_cargs, error);
    
    // Verify that the last error was replaced with overflow error
    cr_assert_eq(test_cargs.error_stack.count, CARGS_MAX_ERRORS_STACK, "Error count should remain at max");
    cr_assert_eq(test_cargs.error_stack.errors[CARGS_MAX_ERRORS_STACK - 1].code, CARGS_ERROR_STACK_OVERFLOW, 
                "Last error should be stack overflow");
}

// Test for CARGS_COLLECT_ERROR macro
Test(error, cargs_collect_error_macro, .init = setup_error)
{
    // Use macro to report an error
    CARGS_COLLECT_ERROR(&test_cargs, CARGS_ERROR_MISSING_VALUE, "Missing value for option '%s'", "test_option");
    
    // Verify if error was collected correctly
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error count should be 1");
    cr_assert_eq(test_cargs.error_stack.errors[0].code, CARGS_ERROR_MISSING_VALUE, "Error code should match");
    cr_assert_str_eq(test_cargs.error_stack.errors[0].message, "Missing value for option 'test_option'", 
                    "Error message should be formatted correctly");
}

// Function that uses CARGS_REPORT_ERROR
int test_function(cargs_t *cargs, const char *opt_name) {
	CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT, "Invalid argument: '%s'", opt_name);
}

// Test for CARGS_REPORT_ERROR macro with stderr redirection
Test(error, cargs_report_error_macro, .init = setup_error, .fini = cr_redirect_stderr)
{    
    // Redirect stderr to capture output
    cr_redirect_stderr();
    
    // Call function that reports error
    int result = test_function(&test_cargs, "test_option");
    
    // Verify that function returned correct error code
    cr_assert_eq(result, CARGS_ERROR_INVALID_ARGUMENT, "Should return the error code");
    
    // Verify that error counter was incremented
    cr_assert_eq(test_cargs.error_stack.count, 1, "Error count should be incremented");
    
    // Note: Verifying stderr content would require additional configuration
    // and is not included in this basic test
}
