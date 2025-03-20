#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "cargs.h"

// Basic usage example options
CARGS_OPTIONS(
    basic_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
    OPTION_INT('p', "port", "Port number", DEFAULT(8080), RANGE(1, 65535)),
    POSITIONAL_STRING("input", "Input file")
)

// Test successful parsing and execution
Test(basic_usage, successful_parse)
{
    // Initialize arguments
    char *argv[] = {
        "program",
        "--verbose",
        "--output=custom.txt",
        "--port=9000",
        "input.txt"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    // Initialize cargs
    cargs_t cargs = cargs_init(basic_options, "program", "1.0.0");
    
    // Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    
    // Assert parsing succeeded
    cr_assert_eq(status, CARGS_SUCCESS, "Parsing failed with status %d", status);
    
    // Check values
    cr_assert_eq(cargs_get_value(cargs, "verbose").as_bool, true);
    cr_assert_str_eq(cargs_get_value(cargs, "output").as_string, "custom.txt");
    cr_assert_eq(cargs_get_value(cargs, "port").as_int, 9000);
    cr_assert_str_eq(cargs_get_value(cargs, "input").as_string, "input.txt");
    
    // Clean up
    cargs_free(&cargs);
}

// Test help display
Test(basic_usage, display_help, .init = cr_redirect_stdout)
{
    // Initialize arguments
    char *argv[] = {"program", "--help"};
    int argc = sizeof(argv) / sizeof(argv[0]);
	(void)argc;

    // Initialize cargs
    cargs_t cargs = cargs_init(basic_options, "program", "1.0.0");
    cargs.description = "Test program description";

	cargs_parse(&cargs, argc, argv);

    // We expect this to exit, but we'll catch the exit with Criterion's special assertion
    cr_assert_fail(); // Should not reach this line
}

// Helper function to set up error testing
void setup_error_case(void)
{
    // Redirect stderr to capture error messages
    cr_redirect_stderr();
}

// Test required argument missing
Test(basic_usage, missing_required, .init = setup_error_case)
{
    // Initialize arguments (missing required positional arg)
    char *argv[] = {"program", "--verbose"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    // Initialize cargs
    cargs_t cargs = cargs_init(basic_options, "program", "1.0.0");
    
    // Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    
    // Assert parsing failed with appropriate error
    cr_assert_neq(status, CARGS_SUCCESS);
    
    // Clean up
    cargs_free(&cargs);
}
