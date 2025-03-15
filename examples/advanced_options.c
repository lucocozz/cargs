/**
 * Advanced options example for cargs
 * 
 * Demonstrates dependencies (REQUIRES), conflicts (CONFLICTS),
 * and exclusive option groups (FLAG_EXCLUSIVE)
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Required option
    OPTION_STRING('i', "input", "Input file", FLAGS(FLAG_REQUIRED)),
    
    // Compression options in an exclusive group (only one can be selected)
    GROUP_START("Compression", GROUP_DESC("Compression options"), 
                FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('z', "gzip", "Use gzip compression"),
        OPTION_FLAG('j', "bzip2", "Use bzip2 compression"),
        OPTION_FLAG('Z', "lzma", "Use lzma compression"),
    GROUP_END(),
    
    // Compression level requires one of the compression options
    OPTION_INT('l', "level", "Compression level", 
              DEFAULT(6), RANGE(1, 9),
              REQUIRES("gzip", "bzip2", "lzma")),
    
    // Options that conflict with each other
    OPTION_FLAG('v', "verbose", "Enable verbose output", 
               CONFLICTS("quiet")),
    OPTION_FLAG('q', "quiet", "Suppress all output", 
               CONFLICTS("verbose")),
    
    // Options that require each other
    OPTION_STRING('u', "username", "Username for authentication", 
                 REQUIRES("password")),
    OPTION_STRING('p', "password", "Password for authentication", 
                 REQUIRES("username"))
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "advanced_options", "1.0.0", 
                              "Example of advanced options");
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access input
    const char* input = cargs_get_value(cargs, "input").as_string;
    printf("Processing file: %s\n", input);
    
    // Check compression options
    if (cargs_is_set(cargs, "gzip")) {
        printf("Using gzip compression");
    } else if (cargs_is_set(cargs, "bzip2")) {
        printf("Using bzip2 compression");
    } else if (cargs_is_set(cargs, "lzma")) {
        printf("Using lzma compression");
    } else {
        printf("No compression selected");
    }
    
    // Show compression level if any compression is selected
    if (cargs_is_set(cargs, "gzip") || 
        cargs_is_set(cargs, "bzip2") || 
        cargs_is_set(cargs, "lzma")) {
        int level = cargs_get_value(cargs, "level").as_int;
        printf(" (level %d)\n", level);
    } else {
        printf("\n");
    }
    
    // Check authentication
    if (cargs_is_set(cargs, "username")) {
        const char* username = cargs_get_value(cargs, "username").as_string;
        printf("Authenticated as: %s\n", username);
    }
    
    // Check output mode
    if (cargs_is_set(cargs, "verbose")) {
        printf("Verbose mode enabled\n");
    } else if (cargs_is_set(cargs, "quiet")) {
        // In a real app, we would suppress output
    }
    
    cargs_free(&cargs);
    return 0;
}
