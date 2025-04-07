/**
 * Positional arguments example for cargs
 * 
 * Demonstrates how to use required and optional positional arguments
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),

    // Optional flag
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),

    // Required positional arguments must come before optional ones
    POSITIONAL_STRING("source", HELP("Source file"), 
                    HINT("SRC")),
    
    POSITIONAL_STRING("destination", HELP("Destination file"), 
                    HINT("DEST")),
    
    // Optional positional argument with default value
    POSITIONAL_INT("buffer_size", HELP("Buffer size in KB"),
                    FLAGS(FLAG_OPTIONAL),
                    DEFAULT(64),
                    RANGE(1, 8192)),
    
    // Optional positional argument without default value
    POSITIONAL_STRING("log_file", HELP("Log file"),
                    FLAGS(FLAG_OPTIONAL),
                    HINT("LOG"))
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "positional_example", "1.0.0");
    cargs.description = "Example of positional arguments";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access required positional arguments
    const char* source = cargs_get(cargs, "source").as_string;
    const char* destination = cargs_get(cargs, "destination").as_string;
    
    // Access optional positional arguments
    int buffer_size = cargs_get(cargs, "buffer_size").as_int;
    
    // Optional argument may not be set
    const char* log_file = cargs_is_set(cargs, "log_file") ? 
                          cargs_get(cargs, "log_file").as_string : 
                          "(none)";
    
    printf("Configuration:\n");
    printf("  Source: %s\n", source);
    printf("  Destination: %s\n", destination);
    printf("  Buffer size: %d KB\n", buffer_size);
    printf("  Log file: %s\n", log_file);
    
    cargs_free(&cargs);
    return 0;
}
