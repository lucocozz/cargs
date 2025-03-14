/**
 * Basic usage example of cargs library
 * 
 * This example demonstrates how to use cargs for parsing command-line arguments
 * with standard options like:
 *   - Flag options (-v, --verbose)
 *   - Integer options (-p 8080, --port=8080)
 *   - String options (-o file.txt, --output=file.txt)
 */

#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt"), HINT("file")),
    OPTION_INT('r', "retries", "Number of connection retries", DEFAULT(3), RANGE(0, 10)),
    GROUP_START("Server options"),
        OPTION_STRING('H', "host", "Server hostname", DEFAULT("localhost"), HINT("hostname"), CHOICES_STRING("localhost", "0.0.0.0")),
        OPTION_INT('p', "port", "Port number", DEFAULT(8080), CHOICES_INT(80, 443, 8080)),
    GROUP_END(),
    POSITIONAL_STRING("input", "Input file", FLAGS(FLAG_REQUIRED)),
    POSITIONAL_STRING("output", "Output file")
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "basic_example", "1.0.0", "Basic example of cargs library");

    // Parse command-line arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS)
        return (status);

    // Access parsed values
    bool verbose = cargs_get_value(cargs, "verbose").as_bool;
    int port = cargs_get_value(cargs, "port").as_int;
    const char* output = cargs_get_value(cargs, "output").as_string;
    int retries = cargs_get_value(cargs, "retries").as_int;

    // Use the parsed values
    printf("Configuration:\n");
    printf("  Verbose mode: %s\n", verbose ? "enabled" : "disabled");
    printf("  Port number: %d\n", port);
    printf("  Output file: %s\n", output);
    printf("  Retries: %d\n", retries);

    // Free resources
    cargs_free(&cargs);
    return (0);
}
