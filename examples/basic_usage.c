/**
 * Basic usage example for cargs library
 * 
 * Demonstrates core features: flags, options with values, positional arguments,
 * and options with only short name or only long name
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Define options
CARGS_OPTIONS(
    options,
    // Standard options
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Flag option with both short and long names
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    
    // String option with only long name (no short name)
    OPTION_STRING(0, "output", "Output file", 
                 DEFAULT("output.txt"), 
                 HINT("FILE")),
    
    // Integer option with only short name (no long name)
    OPTION_INT('p', NULL, "Port number", 
               DEFAULT(8080), 
               RANGE(1, 65535)),
               
    // Another option with only long name (using '\0' instead of 0)
    OPTION_FLAG('\0', "dry-run", "Run without making changes"),
    
    // Required positional argument
    POSITIONAL_STRING("input", "Input file")
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "basic_example", "1.0.0");
    cargs.description = "Basic example of cargs library";

    // Parse command-line arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }

    // Access parsed values
    bool verbose = cargs_get_value(cargs, "verbose").as_bool;
    const char* output = cargs_get_value(cargs, "output").as_string;
    int port = cargs_get_value(cargs, "p").as_int;  // Using short name as ID when only short name exists
    bool dry_run = cargs_get_value(cargs, "dry-run").as_bool;
    const char* input = cargs_get_value(cargs, "input").as_string;

    // Display configuration
    printf("Configuration:\n");
    printf("  Verbose (-v, --verbose): %s\n", verbose ? "enabled" : "disabled");
    printf("  Output (--output only): %s\n", output);
    printf("  Port (-p only): %d\n", port);
    printf("  Dry run (--dry-run only): %s\n", dry_run ? "enabled" : "disabled");
    printf("  Input: %s\n", input);

    // Free resources
    cargs_free(&cargs);
    return 0;
}
