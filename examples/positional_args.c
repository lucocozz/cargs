/**
 * Positional arguments example of cargs library
 * 
 * This example demonstrates how to use cargs to handle positional arguments
 * in addition to regular options.
 */

#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    
    // Positional arguments
    POSITIONAL_STRING("source", "Source file", FLAGS(FLAG_REQUIRED)),
    POSITIONAL_STRING("destination", "Destination file", FLAGS(FLAG_REQUIRED)),
    // POSITIONAL_INT("file_size", "File size in KB", DEFAULT(1024), FLAGS(FLAG_REQUIRED)),
    POSITIONAL_INT("buffer_size", "Buffer size in KB", DEFAULT(64))
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "positional_example", "1.0.0", 
                             "Example of cargs library with positional arguments");
    
    // Parse command-line arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    bool verbose = cargs_get_value(cargs, "verbose").as_bool;
    const char* source = cargs_get_value(cargs, "source").as_string;
    const char* destination = cargs_get_value(cargs, "destination").as_string;
    int buffer_size = cargs_get_value(cargs, "buffer_size").as_int;
    
    // Use the parsed values
    printf("Copy Configuration:\n");
    if (verbose) printf("  [Verbose mode enabled]\n");
    printf("  Source file: %s\n", source);
    printf("  Destination file: %s\n", destination);
    printf("  Buffer size: %d KB\n", buffer_size);
    
    printf("\nExample invocation:\n");
    printf("  %s [-v] source.txt destination.txt [buffer_size]\n", argv[0]);
    
    // Free resources
    cargs_free(&cargs);
    return 0;
}
