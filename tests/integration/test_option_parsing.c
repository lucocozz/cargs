#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include "cargs.h"


// Define test options
CARGS_OPTIONS(
    test_options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('s', "string", "String option", DEFAULT("default")),
    OPTION_INT('i', "int", "Integer option", DEFAULT(42)),
    OPTION_FLAG('f', "flag", "Flag option")
)

// Test with short options
Test(option_parsing, short_options)
{
    // Prepare arguments
    char *argv[] = {"test", "-s", "test_value", "-i", "123", "-f"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    // Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    
    // Assert parsing succeeded
    cr_assert_eq(status, CARGS_SUCCESS, "Parsing failed with status %d", status);
    
    // Assert values were set correctly
    cr_assert_str_eq(cargs_get(cargs, "string").as_string, "test_value");
    cr_assert_eq(cargs_get(cargs, "int").as_int, 123);
    cr_assert_eq(cargs_get(cargs, "flag").as_bool, true);
}
