#include <criterion/criterion.h>
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/parsing.h"
#include "cargs/internal/context.h"
#include "cargs.h"

// Options for tests
CARGS_OPTIONS(
    parse_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    OPTION_INT('c', "count", HELP("Count value")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

// Options for testing subcommands
CARGS_OPTIONS(
    sub_parse_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('d', "debug", HELP("Debug mode"))
)

CARGS_OPTIONS(
    cmd_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    SUBCOMMAND("sub", sub_parse_options, 
               HELP("Subcommand"))
)

// Cargs context for tests
static cargs_t test_cargs;

void setup_parsing(void)
{
    test_cargs.program_name = "test_program";
    test_cargs.options = parse_options;
    test_cargs.error_stack.count = 0;
    context_init(&test_cargs);
}

void setup_subcmd_parsing(void)
{
    test_cargs.program_name = "test_program";
    test_cargs.options = cmd_options;
    test_cargs.error_stack.count = 0;
    context_init(&test_cargs);
}

// Test for handle_long_option
Test(parsing, handle_long_option, .init = setup_parsing)
{
    char *argv[] = {"--output=test.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    int current_index = 0;
    
    // Handle long option with value
    int result = handle_long_option(&test_cargs, parse_options, "output=test.txt", argv, argc, &current_index);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Long option with value should be handled successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    
    // Verify that option was correctly set
    cargs_option_t *option = find_option_by_name(parse_options, "output");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "test.txt", "Option value should be set correctly");
}

// Test for handle_short_option
Test(parsing, handle_short_option, .init = setup_parsing)
{
    char *argv[] = {"program", "-o", "test.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    int current_index = 1;
    
    // Handle short option with value
    int result = handle_short_option(&test_cargs, parse_options, "o", argv, argc, &current_index);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Short option with value should be handled successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    cr_assert_eq(current_index, 2, "Index should be advanced to next argument");
    
    // Verify that option was correctly set
    cargs_option_t *option = find_option_by_name(parse_options, "output");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "test.txt", "Option value should be set correctly");
}

// Test for handle_positional
Test(parsing, handle_positional, .init = setup_parsing)
{
    // Handle positional argument
    int result = handle_positional(&test_cargs, parse_options, "input.txt", 0);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Positional argument should be handled successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    
    // Verify that option was correctly set
    cargs_option_t *option = find_option_by_name(parse_options, "input");
    cr_assert_eq(option->is_set, true, "Option should be marked as set");
    cr_assert_str_eq(option->value.as_string, "input.txt", "Option value should be set correctly");
    
    // Test invalid positional index
    result = handle_positional(&test_cargs, parse_options, "extra.txt", 1);
    cr_assert_neq(result, CARGS_SUCCESS, "Invalid positional index should fail");
}

// Test for handle_subcommand
Test(parsing, handle_subcommand, .init = setup_subcmd_parsing)
{
    char *argv[] = {"-d"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Find subcommand to use
    cargs_option_t *subcmd = find_subcommand(cmd_options, "sub");
    cr_assert_not_null(subcmd, "Subcommand should exist");
    
    // Handle subcommand
    int result = handle_subcommand(&test_cargs, subcmd, argc, argv);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Subcommand should be handled successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    cr_assert_eq(test_cargs.context.subcommand_depth, 1, "Subcommand depth should be 1");
    cr_assert_eq(test_cargs.context.subcommand_stack[0], subcmd, "Subcommand should be on the stack");
    
    // Verify that subcommand option was set
    cargs_option_t *debug_option = find_option_by_name(sub_parse_options, "debug");
    cr_assert_eq(debug_option->is_set, true, "Subcommand option should be set");
}

// Simple test for parse_args
Test(parsing, parse_args_basic, .init = setup_parsing)
{
    char *argv[] = {"program", "-v", "-o", "output.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_cargs, parse_options, argc - 1, &argv[1]);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Arguments should be parsed successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    cargs_option_t *verbose = find_option_by_name(parse_options, "verbose");
    cargs_option_t *output = find_option_by_name(parse_options, "output");
    cargs_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(verbose->value.as_bool, true, "Verbose value should be true");
    
    cr_assert_eq(output->is_set, true, "Output option should be set");
    cr_assert_str_eq(output->value.as_string, "output.txt", "Output value should be correct");
    
    cr_assert_eq(input->is_set, true, "Input option should be set");
    cr_assert_str_eq(input->value.as_string, "input.txt", "Input value should be correct");
}

// Test for parse_args with long options
Test(parsing, parse_args_long_options, .init = setup_parsing)
{
    char *argv[] = {"program", "--verbose", "--output=output.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_cargs, parse_options, argc - 1, &argv[1]);
    
    cr_assert_eq(result, CARGS_SUCCESS, "Arguments with long options should be parsed successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    cargs_option_t *verbose = find_option_by_name(parse_options, "verbose");
    cargs_option_t *output = find_option_by_name(parse_options, "output");
    cargs_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(output->is_set, true, "Output option should be set");
    cr_assert_str_eq(output->value.as_string, "output.txt", "Output value should be correct");
    cr_assert_eq(input->is_set, true, "Input option should be set");
}

// Test for parse_args with -- (end of options)
Test(parsing, parse_args_end_options, .init = setup_parsing)
{
    char *argv[] = {"program", "-v", "--", "-o"};
    int argc = sizeof(argv) / sizeof(char *);
    
    // Parse arguments
    int result = parse_args(&test_cargs, parse_options, argc - 1, &argv[1]);

    cr_assert_eq(result, CARGS_SUCCESS, "Arguments with -- should be parsed successfully");
    cr_assert_eq(test_cargs.error_stack.count, 0, "No errors should be reported");
    
    // Verify that options were correctly set
    cargs_option_t *verbose = find_option_by_name(parse_options, "verbose");
    cargs_option_t *output = find_option_by_name(parse_options, "output");
    cargs_option_t *input = find_option_by_name(parse_options, "input");
    
    cr_assert_eq(verbose->is_set, true, "Verbose option should be set");
    cr_assert_eq(output->is_set, false, "Output option should not be set");
    cr_assert_eq(input->is_set, true, "Input option should be set");
    cr_assert_str_eq(input->value.as_string, "-o", "Input value should be -o");
}
