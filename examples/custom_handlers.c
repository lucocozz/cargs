/**
 * Custom handlers example of cargs library
 * 
 * This example demonstrates how to create and use custom handlers
 * for special option types or validation.
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Custom handler for IPv4 address validation
int ipv4_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    (void)cargs;  // Unused parameter
    if (arg == NULL) {
        fprintf(stderr, "Error: IP address is required\n");
        return CARGS_ERROR_MISSING_VALUE;
    }

    // Extremely simple IPv4 validation - in real code, you would use more robust validation
    int a, b, c, d;
    if (sscanf(arg, "%d.%d.%d.%d", &a, &b, &c, &d) != 4 ||
        a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) {
        fprintf(stderr, "Error: Invalid IPv4 address format: %s\n", arg);
        return CARGS_ERROR_INVALID_FORMAT;
    }

    option->value.as_string = arg;
    return CARGS_SUCCESS;
}

// Custom handler for log level (debug, info, warning, error)
int log_level_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    (void)cargs;  // Unused parameter
    if (arg == NULL) {
        fprintf(stderr, "Error: Log level is required\n");
        return CARGS_ERROR_MISSING_VALUE;
    }

    // Convert string to lowercase for case-insensitive comparison
    char* level = strdup(arg);
    for (char* p = level; *p; ++p) {
        *p = tolower(*p);
    }

    int value;
    if (strcmp(level, "debug") == 0) value = 0;
    else if (strcmp(level, "info") == 0) value = 1;
    else if (strcmp(level, "warning") == 0) value = 2;
    else if (strcmp(level, "error") == 0) value = 3;
    else {
        free(level);
        fprintf(stderr, "Error: Invalid log level: %s (valid values: debug, info, warning, error)\n", arg);
        return CARGS_ERROR_INVALID_VALUE;
    }

    free(level);
    option->value.as_int = value;
    return CARGS_SUCCESS;
}

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Option with custom IP address handler
    OPTION_BASE('i', "ip", "Server IP address", VALUE_TYPE_STRING, HANDLER(ipv4_handler)),
    
    // Option with custom log level handler
    OPTION_BASE('l', "log-level", "Log level (debug, info, warning, error)", VALUE_TYPE_INT, 
                HANDLER(log_level_handler), DEFAULT(1))  // Default to INFO
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "custom_handlers_example", "1.0.0", 
                             "Example of cargs library with custom handlers");
    
    // Parse command-line arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    const char* ip = cargs_get_value(cargs, "ip").as_string;
    int log_level = cargs_get_value(cargs, "log-level").as_int;
    
    // Display log level as string
    const char* log_level_str;
    switch (log_level) {
        case 0: log_level_str = "DEBUG"; break;
        case 1: log_level_str = "INFO"; break;
        case 2: log_level_str = "WARNING"; break;
        case 3: log_level_str = "ERROR"; break;
        default: log_level_str = "UNKNOWN";
    }
    
    // Use the parsed values
    printf("Configuration:\n");
    if (ip) printf("  Server IP: %s\n", ip);
    printf("  Log level: %s (%d)\n", log_level_str, log_level);
    
    // Free resources
    cargs_free(&cargs);
    return 0;
}
