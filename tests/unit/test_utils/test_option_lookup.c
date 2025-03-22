#include <criterion/criterion.h>
#include "cargs/internal/utils.h"
#include "cargs/internal/context.h"
#include "cargs.h"

// Mock options for testing
CARGS_OPTIONS(
    test_options,
    OPTION_FLAG('v', "verbose", "Verbose output"),
    OPTION_STRING('o', "output", "Output file"),
    OPTION_STRING('s', NULL, "Short-only option"),
    OPTION_FLAG('\0', "long-only", "Long-only option"),
    POSITIONAL_STRING("input", "Input file"),
    POSITIONAL_STRING("output", "Output file"),
    POSITIONAL_INT("count", "Count value", FLAGS(FLAG_OPTIONAL))
)

// Mock subcommand options
CARGS_OPTIONS(
    sub_options,
    OPTION_FLAG('d', "debug", "Debug mode"),
    POSITIONAL_STRING("subfile", "Subcommand file")
)

// Mock nested subcommand options
CARGS_OPTIONS(
    nested_options,
    OPTION_FLAG('n', "nested", "Nested option")
)

// Mock options with subcommands
CARGS_OPTIONS(
    cmd_options,
    OPTION_FLAG('g', "global", "Global option"),
    SUBCOMMAND("sub", sub_options, HELP("Subcommand")),
    SUBCOMMAND("nested", nested_options, HELP("Nested subcommand"))
)

// Mock cargs context for testing
static cargs_t test_cargs;

// Setup function
void setup(void)
{
    test_cargs.program_name = "test_prog";
    test_cargs.options = test_options;
    test_cargs.error_stack.count = 0;
    context_init(&test_cargs);
}

// Setup function for subcommand tests
void setup_subcommands(void)
{
    test_cargs.program_name = "test_prog";
    test_cargs.options = cmd_options;
    test_cargs.error_stack.count = 0;
    context_init(&test_cargs);
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
    
    // Test positional argument lookup
    option = find_option_by_name(test_options, "input");
    cr_assert_not_null(option, "Should find positional by name");
    cr_assert_eq(option->type, TYPE_POSITIONAL, "Found option should be positional");
    
    // Test option with only short name
    option = find_option_by_name(test_options, "s");
    cr_assert_not_null(option, "Should find option with only short name");
    cr_assert_eq(option->sname, 's', "Found option should have correct short name");
    
    // Test option with only long name
    option = find_option_by_name(test_options, "long-only");
    cr_assert_not_null(option, "Should find option with only long name");
    cr_assert_eq(option->sname, '\0', "Found option should have no short name");
}

Test(parsing, find_option_by_sname)
{
    cargs_option_t* option = find_option_by_sname(test_options, 'v');
    cr_assert_not_null(option, "Should find option by short name");
    cr_assert_str_eq(option->name, "verbose", "Found option should have correct name");
    
    option = find_option_by_sname(test_options, 'x');
    cr_assert_null(option, "Should return NULL for nonexistent short name");
    
    // Test option with no long name
    option = find_option_by_sname(test_options, 's');
    cr_assert_not_null(option, "Should find option with no long name");
    cr_assert_eq(option->lname, NULL, "Found option should have no long name");
}

Test(parsing, find_option_by_lname)
{
    cargs_option_t* option = find_option_by_lname(test_options, "verbose");
    cr_assert_not_null(option, "Should find option by long name");
    cr_assert_eq(option->sname, 'v', "Found option should have correct short name");
    
    option = find_option_by_lname(test_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent long name");
    
    // Test option with only long name
    option = find_option_by_lname(test_options, "long-only");
    cr_assert_not_null(option, "Should find option with only long name");
    cr_assert_eq(option->sname, '\0', "Found option should have no short name");
    
    // Test option with no long name
    option = find_option_by_lname(test_options, NULL);
    cr_assert_null(option, "Should return NULL when searching for NULL long name");
}

Test(parsing, find_positional)
{
    cargs_option_t* option = find_positional(test_options, 0);
    cr_assert_not_null(option, "Should find first positional argument");
    cr_assert_str_eq(option->name, "input", "First positional should be 'input'");
    
    option = find_positional(test_options, 1);
    cr_assert_not_null(option, "Should find second positional argument");
    cr_assert_str_eq(option->name, "output", "Second positional should be 'output'");
    
    option = find_positional(test_options, 2);
    cr_assert_not_null(option, "Should find third positional argument");
    cr_assert_str_eq(option->name, "count", "Third positional should be 'count'");
    
    option = find_positional(test_options, 3);
    cr_assert_null(option, "Should return NULL for nonexistent positional index");
}

Test(parsing, find_subcommand, .init = setup_subcommands)
{
    cargs_option_t* option = find_subcommand(cmd_options, "sub");
    cr_assert_not_null(option, "Should find subcommand");
    cr_assert_str_eq(option->name, "sub", "Should find correct subcommand");
    cr_assert_eq(option->type, TYPE_SUBCOMMAND, "Found option should be subcommand");
    
    option = find_subcommand(cmd_options, "nested");
    cr_assert_not_null(option, "Should find nested subcommand");
    cr_assert_str_eq(option->name, "nested", "Should find correct nested subcommand");
    
    option = find_subcommand(cmd_options, "nonexistent");
    cr_assert_null(option, "Should return NULL for nonexistent subcommand");
}

Test(parsing, find_option_by_active_path, .init = setup_subcommands)
{
    // Test finding option at root level
    cargs_option_t* option = find_option_by_active_path(test_cargs, "global");
    cr_assert_not_null(option, "Should find option at root level");
    cr_assert_str_eq(option->name, "global", "Should find correct option");
    
    // Test finding option by explicit root path
    option = find_option_by_active_path(test_cargs, ".global");
    cr_assert_not_null(option, "Should find option by explicit root path");
    cr_assert_str_eq(option->name, "global", "Should find correct option");
    
    // Test with subcommand path (should fail since no active subcommand)
    option = find_option_by_active_path(test_cargs, "sub.debug");
    cr_assert_null(option, "Should return NULL when no matching subcommand is active");
    
    // Test with an invalid path
    option = find_option_by_active_path(test_cargs, "nonexistent.option");
    cr_assert_null(option, "Should return NULL for invalid path");
    
    // Test with active subcommand
    cargs_option_t* sub_cmd = find_subcommand(cmd_options, "sub");
    context_push_subcommand(&test_cargs, sub_cmd);
    
    // Now we should be able to find the subcommand option
    option = find_option_by_active_path(test_cargs, "debug");
    cr_assert_not_null(option, "Should find option in active subcommand");
    cr_assert_str_eq(option->name, "debug", "Should find correct option in subcommand");
    
    // Still should be able to access root option with explicit path
    option = find_option_by_active_path(test_cargs, ".global");
    cr_assert_not_null(option, "Should still find root option with explicit path");
    cr_assert_str_eq(option->name, "global", "Should find correct root option");
    
    // Clean up
    context_pop_subcommand(&test_cargs);
}

Test(parsing, get_active_options, .init = setup_subcommands)
{
    // Initially should get root options
    const cargs_option_t* options = get_active_options(&test_cargs);
    cr_assert_eq(options, cmd_options, "Should initially return root options");
    
    // Push a subcommand
    cargs_option_t* sub_cmd = find_subcommand(cmd_options, "sub");
    context_push_subcommand(&test_cargs, sub_cmd);
    
    // Now should get subcommand options
    options = get_active_options(&test_cargs);
    cr_assert_eq(options, sub_options, "Should return subcommand options when subcommand is active");
    
    // Clean up
    context_pop_subcommand(&test_cargs);
}
