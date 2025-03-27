#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <criterion/parameterized.h>
#include <time.h>
#include <stdio.h>
#include "cargs.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"

// Valid options definition for testing
CARGS_OPTIONS(
    valid_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Verbose output"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
    OPTION_INT('c', "count", "Counter value", RANGE(1, 100), DEFAULT(1)),
    POSITIONAL_STRING("input", "Input file", FLAGS(FLAG_REQUIRED))
)

// Invalid options definition (missing help option - should fail validation)
CARGS_OPTIONS(
    invalid_options,
    OPTION_FLAG('v', "verbose", "Verbose output"),
    OPTION_STRING('o', "output", "Output file"),
    POSITIONAL_STRING("input", "Input file")
)

// Invalid options with duplicated option names
CARGS_OPTIONS(
    duplicate_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Verbose output"),
    OPTION_STRING('v', "verbose", "Duplicate option")  // Same names
)

// Helper function for measuring initialization time
double measure_init_time(cargs_option_t *options, const char *program_name, 
                         const char *version, int iterations) 
{
    clock_t start, end;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; i++) {
        start = clock();
        
        // Initialize cargs
        cargs_t cargs = cargs_init(options, program_name, version);
        
        end = clock();
        total_time += ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Clean up
        cargs_free(&cargs);
    }
    
    return total_time / iterations;
}

#ifdef CARGS_RELEASE
Test(release_mode, init_skips_validation_with_invalid_options)
{
    // With CARGS_RELEASE defined, initialization should succeed even with invalid options
    cargs_t cargs = cargs_init(invalid_options, "test_program", "1.0.0");
    
    // In release mode, the initialization shouldn't perform validation
    // so no errors should be reported
    cr_assert_eq(cargs.error_stack.count, 0, 
                 "No errors should be reported in release mode");
                 
    // Even options with duplicate names should initialize without errors
    cargs_t cargs2 = cargs_init(duplicate_options, "test_program", "1.0.0");
    cr_assert_eq(cargs2.error_stack.count, 0, 
                 "No errors should be reported for duplicate options in release mode");
    
    cargs_free(&cargs);
    cargs_free(&cargs2);
}

Test(release_mode, parse_works_correctly_with_valid_options)
{
    // Test that parsing still works correctly in release mode
    cargs_t cargs = cargs_init(valid_options, "test_program", "1.0.0");
    
    char *argv[] = {"test_program", "--verbose", "--output=test.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Parsing should succeed in release mode");
    cr_assert_eq(cargs_is_set(cargs, "verbose"), true, "Verbose option should be set");
    cr_assert_str_eq(cargs_get(cargs, "output").as_string, "test.txt", "Output should be correctly set");
    cr_assert_str_eq(cargs_get(cargs, "input").as_string, "input.txt", "Input should be correctly set");
    
    cargs_free(&cargs);
}
#else
// In this test we expect the program to exit, so we don't need the assert_fail
// The .exit_code attribute will handle the expected behavior
Test(release_mode, init_performs_validation_with_invalid_options, .exit_code = 1, .init = cr_redirect_stderr)
{
    // This will exit due to validation errors, which will make the test pass
    // because of the .exit_code = 1 attribute
    cargs_init(invalid_options, "test_program", "1.0.0");
    
    // We should never reach this point, but we need something here
    // to avoid compiler warnings about empty functions
    cr_assert(false, "Should not reach this point");
}

// Same here, the test passes if the program exits with the specified code
Test(release_mode, init_performs_validation_with_duplicate_options, .exit_code = 1, .init = cr_redirect_stderr)
{
    // This will exit due to validation errors, which will make the test pass
    cargs_init(duplicate_options, "test_program", "1.0.0");
    
    // We should never reach this point
    cr_assert(false, "Should not reach this point");
}

Test(release_mode, init_succeeds_with_valid_options)
{
    // With validation enabled, initialization should succeed with valid options
    cargs_t cargs = cargs_init(valid_options, "test_program", "1.0.0");
    
    // No errors should be reported for valid options
    cr_assert_eq(cargs.error_stack.count, 0, 
                 "No errors should be reported for valid options");
    
    cargs_free(&cargs);
}
#endif

// This test is always run and measures performance
Test(release_mode, performance_comparison)
{
    // Number of iterations for reliable measurement
    const int iterations = 1000;
    
    // Measure initialization time with valid options
    double time_valid = measure_init_time(valid_options, "test_program", "1.0.0", iterations);
    
#ifdef CARGS_RELEASE
    // In release mode, we can measure invalid options too (validation is skipped)
    double time_invalid = measure_init_time(invalid_options, "test_program", "1.0.0", iterations);
    double time_duplicate = measure_init_time(duplicate_options, "test_program", "1.0.0", iterations);
    
    cr_log_info("Performance in RELEASE mode (average over %d iterations):", iterations);
    cr_log_info("  Valid options: %.9f seconds", time_valid);
    cr_log_info("  Invalid options: %.9f seconds", time_invalid);
    cr_log_info("  Duplicate options: %.9f seconds", time_duplicate);
    
    // In release mode, all initializations should be roughly the same speed
    cr_assert_float_eq(time_valid, time_invalid, 0.0001, 
                     "Release mode should have similar performance for valid and invalid options");
#else
    cr_log_info("Performance in DEBUG mode (average over %d iterations):", iterations);
    cr_log_info("  Valid options: %.9f seconds", time_valid);
    
    // In debug mode, we can still assert that valid options initialize in a reasonable time
    cr_assert_lt(time_valid, 0.001, 
                "Even with validation, initialization should be reasonably fast for valid options");
#endif
}
