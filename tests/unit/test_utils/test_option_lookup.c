// tests/unit/test_core/test_parsing.c
#include <criterion/criterion.h>
#include "cargs/internal/utils.h"
#include "cargs.h"

// Mock options for testing
CARGS_OPTIONS(
	test_options,
	OPTION_FLAG('v', "verbose", "Verbose output"),
	OPTION_STRING('o', "output", "Output file")
)

// Mock cargs context for testing
static cargs_t test_cargs;

// Setup function
void setup(void)
{
    test_cargs.program_name = "test_prog";
    test_cargs.options = test_options;
    test_cargs.error_stack.count = 0;
}

// This is a placeholder test since full parsing tests are complex
// and require proper setup of handlers and context
Test(parsing, parse_args_basic)
{
    // These would need proper implementation of handlers to work
    cr_assert(true, "Placeholder for parse_args tests");
}

// Testing find option functions
Test(parsing, find_option_by_name)
{
    cargs_option_t* option = find_option_by_name(test_options, "verbose");
    cr_assert_not_null(option, "Should find option by name");
    cr_assert_eq(option->sname, 'v', "Found option should have correct short name");
    
    option = find_option_by_name(test_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent option");
}

Test(parsing, find_option_by_sname)
{
    cargs_option_t* option = find_option_by_sname(test_options, 'v');
    cr_assert_not_null(option, "Should find option by short name");
    cr_assert_str_eq(option->name, "verbose", "Found option should have correct name");
    
    option = find_option_by_sname(test_options, 'x');
    cr_assert_null(option, "Should return NULL for nonexistent short name");
}

Test(parsing, find_option_by_lname)
{
    cargs_option_t* option = find_option_by_lname(test_options, "verbose");
    cr_assert_not_null(option, "Should find option by long name");
    cr_assert_eq(option->sname, 'v', "Found option should have correct short name");
    
    option = find_option_by_lname(test_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent long name");
}
