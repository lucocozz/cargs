/**
 * Subcommands example for cargs
 * 
 * Demonstrates how to implement command structures similar to git/docker
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Subcommand action handlers
int add_command(cargs_t *cargs, void *data);
int remove_command(cargs_t *cargs, void *data);

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
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = cargs_get(*cargs, "verbose").as_bool;
    
    // Get command-specific options
    const char* file = cargs_get(*cargs, "add.file").as_string;
    bool force = cargs_get(*cargs, "add.force").as_bool;

    printf("Adding file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (force) printf("  with force option\n");

    return 0;
}

// Implementation of the "remove" command
int remove_command(cargs_t *cargs, void *data)
{
    (void)data; // Unused parameter
    
    // Get the global option
    bool verbose = cargs_get(*cargs, "verbose").as_bool;
    
    // Get command-specific options
    const char* file = cargs_get(*cargs, "rm.file").as_string;
    bool recursive = cargs_get(*cargs, "rm.recursive").as_bool;

    printf("Removing file: %s\n", file);
    if (verbose) printf("  verbose mode enabled\n");
    if (recursive) printf("  recursively\n");

    return 0;
}

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "subcommands_example", "1.0.0");
    cargs.description = "Example of subcommands";

    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }

    if (cargs_has_command(cargs)) {
        // Execute the subcommand handler
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }

    cargs_free(&cargs);
    return 0;
}
