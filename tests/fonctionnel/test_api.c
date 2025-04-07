#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "cargs.h"
#include <stdlib.h>
#include <string.h>

// Test options
CARGS_OPTIONS(
    api_test_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('n', "number", HELP("A number"), DEFAULT(42)),
    OPTION_ARRAY_INT('a', "array", HELP("An array of numbers")),
    OPTION_MAP_INT('m', "map", HELP("A map of numbers")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

// Subcommand options
CARGS_OPTIONS(
    api_sub_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('d', "debug", HELP("Debug mode"))
)

// Test action handler
int test_action_called = 0;
int test_action(cargs_t *cargs, void *data) {
    (void)data;
    (void)cargs;
    test_action_called = 1;
    return 0;
}

// Options with subcommand
CARGS_OPTIONS(
    api_cmd_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("sub", api_sub_options, HELP("Subcommand"), ACTION(test_action))
)

// Test cargs_init
Test(api, cargs_init)
{
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");

    // Check that cargs was initialized correctly
    cr_assert_str_eq(cargs.program_name, "test_program", "Program name should be set correctly");
    cr_assert_str_eq(cargs.version, "1.0.0", "Version should be set correctly");
    cr_assert_eq(cargs.options, api_test_options, "Options should be set correctly");
    cr_assert_eq(cargs.error_stack.count, 0, "Error stack should be empty");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_parse with valid arguments
Test(api, cargs_parse_valid)
{
    char *argv[] = {"test_program", "-v", "-o", "custom.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    // Check that parsing succeeded
    cr_assert_eq(status, CARGS_SUCCESS, "Parsing valid arguments should succeed");
    
    // Check that options were set correctly
    cr_assert_eq(cargs_get(cargs, "verbose").as_bool, true, "Verbose option should be set");
    cr_assert_str_eq(cargs_get(cargs, "output").as_string, "custom.txt", "Output option should be set correctly");
    cr_assert_str_eq(cargs_get(cargs, "input").as_string, "input.txt", "Input option should be set correctly");
    
    // Check default values
    cr_assert_eq(cargs_get(cargs, "number").as_int, 42, "Number option should have default value");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_parse with invalid arguments
Test(api, cargs_parse_invalid, .init = cr_redirect_stderr)
{
    // Missing required positional argument
    char *argv[] = {"test_program", "-v"};
    int argc = sizeof(argv) / sizeof(char *);
    
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    // Check that parsing failed
    cr_assert_neq(status, CARGS_SUCCESS, "Parsing invalid arguments should fail");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_is_set function
Test(api, cargs_is_set)
{
    char *argv[] = {"test_program", "-v", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    cargs_parse(&cargs, argc, argv);
    
    // Check options that were set
    cr_assert_eq(cargs_is_set(cargs, "verbose"), true, "Verbose option should be set");
    cr_assert_eq(cargs_is_set(cargs, "input"), true, "Input option should be set");
    
    // Check options that were not set explicitly
    cr_assert_eq(cargs_is_set(cargs, "number"), true, "Number option should be set via default");
    
    // Check non-existent option
    cr_assert_eq(cargs_is_set(cargs, "nonexistent"), false, "Non-existent option should not be set");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_has_command and cargs_exec functions
Test(api, cargs_has_command_and_exec)
{
    char *argv[] = {"test_program", "sub", "-d"};
    int argc = sizeof(argv) / sizeof(char *);
    
    test_action_called = 0; // Reset action flag
    
    cargs_t cargs = cargs_init(api_cmd_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    // Check that parsing succeeded
    cr_assert_eq(status, CARGS_SUCCESS, "Parsing valid arguments should succeed");
    
    // Check that we have a command
    cr_assert_eq(cargs_has_command(cargs), true, "Should have a command");
    
    // Execute the command
    status = cargs_exec(&cargs, NULL);
    
    // Check that execution succeeded
    cr_assert_eq(status, 0, "Command execution should succeed");
    cr_assert_eq(test_action_called, 1, "Command action should have been called");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_get function with different types
Test(api, cargs_get_different_types)
{
    char *argv[] = {"test_program", "-v", "-n", "100", "-o", "file.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    cargs_parse(&cargs, argc, argv);
    
    // Check different value types
    cr_assert_eq(cargs_get(cargs, "verbose").as_bool, true, "Boolean value should be correct");
    cr_assert_eq(cargs_get(cargs, "number").as_int, 100, "Integer value should be correct");
    cr_assert_str_eq(cargs_get(cargs, "output").as_string, "file.txt", "String value should be correct");
    
    // Check non-existent option (should return empty value)
    cr_assert_eq(cargs_get(cargs, "nonexistent").raw, 0, "Non-existent option should return empty value");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_count function
Test(api, cargs_count)
{
    char *argv[] = {"test_program", "--verbose", "input.txt", "--array=1,2,3,4", \
                    "--map=one=1,two=2,three=3"};
    int argc = sizeof(argv) / sizeof(char *);
    
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    cargs_parse(&cargs, argc, argv);

    // Check count for single-value options
    cr_assert_eq(cargs_count(cargs, "input"), 1, "Single value should have count 1");
    cr_assert_eq(cargs_count(cargs, "verbose"), 1, "Single value should have count 1");
    
    // Check count for array option
    cr_assert_eq(cargs_count(cargs, "array"), 4, "Array option should have count 4");

    // Check count for map option
    cr_assert_eq(cargs_count(cargs, "map"), 3, "Map option should have count 3");
    

    // Check count for non-existent option
    cr_assert_eq(cargs_count(cargs, "nonexistent"), 0, "Non-existent option should have count 0");

    // Clean up
    cargs_free(&cargs);
}

// Test cargs_print functions (minimal test to ensure they don't crash)
Test(api, cargs_print_functions, .init = cr_redirect_stdout)
{
    cargs_t cargs = cargs_init(api_test_options, "test_program", "1.0.0");
    cargs.description = "Test program for cargs";
    
    // Test the print functions
    cargs_print_help(cargs);
    cargs_print_usage(cargs);
    cargs_print_version(cargs);
    
    // We don't assert on the output, just make sure they don't crash
    cr_assert(true, "Print functions should not crash");
    
    // Clean up
    cargs_free(&cargs);
}
