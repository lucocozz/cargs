#include <criterion/criterion.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/context.h"
#include "cargs.h"

// External functions to test
int validate_option(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option);
int validate_positional(cargs_t *cargs, cargs_option_t *option);
int validate_structure(cargs_t *cargs, cargs_option_t *options);
int validate_subcommand(cargs_t *cargs, cargs_option_t *option);
int validate_group(cargs_t *cargs, cargs_option_t *option);

// Mock options for tests
CARGS_OPTIONS(
    valid_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

CARGS_OPTIONS(
    invalid_options, // Intentionally invalid, no help option
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"))
)

CARGS_OPTIONS(
    duplicate_options, // Intentionally invalid, duplicate names
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('v', "verbose", HELP("Duplicate option")) // Same name and same short option
)

// Contexte cargs pour les tests
static cargs_t test_cargs;

void setup_validation(void)
{
    test_cargs.program_name = "test_program";
    test_cargs.error_stack.count = 0;
    context_init(&test_cargs);
}

// Test for validating a valid option
Test(validation, validate_valid_option, .init = setup_validation)
{
    cargs_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 't',
        .lname = "test",
        .help = "Test option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_cargs, valid_options, &option);
    cr_assert_eq(result, CARGS_SUCCESS, "Valid option should pass validation");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid option
Test(validation, validate_invalid_option, .init = setup_validation)
{
    // Option without short or long name
    cargs_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 0,
        .lname = NULL,
        .help = "Test option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_cargs, valid_options, &option);
    cr_assert_neq(result, CARGS_SUCCESS, "Option without short or long name should fail validation");
    cr_assert_eq(test_cargs.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid positional option
Test(validation, validate_valid_positional, .init = setup_validation)
{
    cargs_option_t option = {
        .type = TYPE_POSITIONAL,
        .name = "test",
        .help = "Test positional option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    
    int result = validate_positional(&test_cargs, &option);
    cr_assert_eq(result, CARGS_SUCCESS, "Valid positional should pass validation");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid positional option
Test(validation, validate_invalid_positional, .init = setup_validation)
{
    // Positional option without name
    cargs_option_t option = {
        .type = TYPE_POSITIONAL,
        .name = NULL,
        .help = "Test positional option",
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    
    int result = validate_positional(&test_cargs, &option);
    cr_assert_neq(result, CARGS_SUCCESS, "Positional option without name should fail validation");
    cr_assert_eq(test_cargs.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid group
Test(validation, validate_valid_group, .init = setup_validation)
{
    cargs_option_t option = {
        .type = TYPE_GROUP,
        .name = "test_group",
        .help = "Test group"
    };
    
    int result = validate_group(&test_cargs, &option);
    cr_assert_eq(result, CARGS_SUCCESS, "Valid group should pass validation");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
}

// Test for validating a valid subcommand
Test(validation, validate_valid_subcommand, .init = setup_validation)
{
    cargs_option_t option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_cmd",
        .help = "Test subcommand",
        .sub_options = valid_options
    };
    
    int result = validate_subcommand(&test_cargs, &option);
    cr_assert_eq(result, CARGS_SUCCESS, "Valid subcommand should pass validation");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid subcommand
Test(validation, validate_invalid_subcommand, .init = setup_validation)
{
    // Subcommand without options
    cargs_option_t option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_cmd",
        .help = "Test subcommand",
        .sub_options = NULL
    };
    
    int result = validate_subcommand(&test_cargs, &option);
    cr_assert_neq(result, CARGS_SUCCESS, "Subcommand without options should fail validation");
    cr_assert_eq(test_cargs.error_stack.count, 1, "An error should be reported");
}

// Test for validating a valid structure
Test(validation, validate_valid_structure, .init = setup_validation)
{
    int result = validate_structure(&test_cargs, valid_options);
    cr_assert_eq(result, CARGS_SUCCESS, "Valid structure should pass validation");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
}

// Test for validating an invalid structure
Test(validation, validate_invalid_structure, .init = setup_validation)
{
    int result = validate_structure(&test_cargs, invalid_options);
    cr_assert_neq(result, CARGS_SUCCESS, "Structure without help option should fail validation");
    cr_assert_gt(test_cargs.error_stack.count, 0, "Errors should be reported");
}

// Test for validating a structure with duplicate options
Test(validation, validate_duplicate_options, .init = setup_validation)
{
	fprintf(stderr, "validating duplicate options\n");
    int result = validate_structure(&test_cargs, duplicate_options);
	fprintf(stderr, "result: %d\n", result);
	fprintf(stderr, "count: %ld\n", test_cargs.error_stack.count);
    cr_assert_neq(result, CARGS_SUCCESS, "Structure with duplicate options should fail validation");
    cr_assert_gt(test_cargs.error_stack.count, 0, "Errors should be reported");
}

// Test for validating a option without a helper
Test(validation, validate_option_without_help, .init = setup_validation)
{
    // Option without help message
    cargs_option_t option = {
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 't',
        .lname = "test",
        .help = NULL, // No help message
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler
    };
    
    int result = validate_option(&test_cargs, valid_options, &option);
    cr_assert_neq(result, CARGS_SUCCESS, "Option without help should fail validation");
    cr_assert_gt(test_cargs.error_stack.count, 0, "Errors should be reported");

    // positionnal option without help
    cargs_option_t pos_option = {
        .type = TYPE_POSITIONAL,
        .name = "test_pos",
        .help = NULL, // No help message
        .value_type = VALUE_TYPE_STRING,
        .handler = string_handler,
        .flags = FLAG_REQUIRED
    };
    result = validate_positional(&test_cargs, &pos_option);
    cr_assert_neq(result, CARGS_SUCCESS, "Positional option without help should fail validation");
    cr_assert_gt(test_cargs.error_stack.count, 0, "Errors should be reported");

    // subcommand without help
    cargs_option_t sub_option = {
        .type = TYPE_SUBCOMMAND,
        .name = "test_sub",
        .help = NULL, // No help message
        .sub_options = valid_options
    };
    result = validate_subcommand(&test_cargs, &sub_option);
    cr_assert_neq(result, CARGS_SUCCESS, "Subcommand without help should fail validation");
    cr_assert_gt(test_cargs.error_stack.count, 0, "Errors should be reported");
}
