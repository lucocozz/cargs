// tests/integration/test_subcommands.c
#include <criterion/criterion.h>
#include "cargs.h"

// Define action handlers
int add_command(cargs_t *cargs, void *data);
int remove_command(cargs_t *cargs, void *data);

// Track if commands were executed
static bool add_executed = false;
static bool remove_executed = false;

// Define options for "add" subcommand
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", "Force add operation"),
    POSITIONAL_STRING("file", "File to add")
)

// Define options for "remove" subcommand
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", "Recursively remove directories"),
    POSITIONAL_STRING("file", "File to remove")
)

// Define main options with subcommands
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global option applicable to all subcommands
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    
    // Define subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Remove files from the index"), 
               ACTION(remove_command))
)

// Implementation of the "add" command
int add_command(cargs_t *cargs, void *data)
{
    (void)data;
	(void)cargs;
    add_executed = true;
    return 0;
}

// Implementation of the "remove" command
int remove_command(cargs_t *cargs, void *data)
{
    (void)data;
	(void)cargs;
    remove_executed = true;
    return 0;
}

// Reset before each test
void reset(void)
{
    add_executed = false;
    remove_executed = false;
}

// Test basic subcommand parsing
Test(subcommands, basic_parsing, .init = reset)
{
    // Test "add" subcommand
    char *argv[] = {"test", "add", "file.txt"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid subcommand should parse successfully");
    cr_assert(cargs_has_command(cargs), "cargs_has_command should return true");
    cr_assert(cargs_is_set(cargs, "add"), "add subcommand should be set");
    
    // Execute the command
    status = cargs_exec(&cargs, NULL);
    cr_assert_eq(status, 0, "Command execution should succeed");
    cr_assert(add_executed, "Add command should have been executed");
    cr_assert_not(remove_executed, "Remove command should not have been executed");
    
    cargs_free(&cargs);
}

// Test subcommand with options
Test(subcommands, with_options, .init = reset)
{
    // Test "add" subcommand with force option
    char *argv[] = {"test", "add", "--force", "file.txt"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Subcommand with options should parse successfully");
    cr_assert(cargs_is_set(cargs, "add.force"), "Force option should be set");
    cr_assert_str_eq(cargs_get(cargs, "add.file").as_string, "file.txt", "File argument should be parsed");
    
    cargs_free(&cargs);
}

// Test global options with subcommands
Test(subcommands, global_options, .init = reset)
{
    // Test global verbose option with "add" subcommand
    char *argv[] = {"test", "--verbose", "add", "file.txt"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Global option with subcommand should parse successfully");
    cr_assert(cargs_is_set(cargs, "verbose"), "Global verbose option should be set");
    cr_assert(cargs_is_set(cargs, "add"), "Add subcommand should be set");
    
    cargs_free(&cargs);
}
