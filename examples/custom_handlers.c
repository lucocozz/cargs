/**
 * Custom handlers example for cargs
 * 
 * Demonstrates custom handlers for specialized option types
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Log level enumeration
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3
} log_level_t;

// Handler for IPv4 address validation
int ipv4_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    if (arg == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
                          "IP address is required");
    }

    // Simple IPv4 validation
    int a, b, c, d;
    if (sscanf(arg, "%d.%d.%d.%d", &a, &b, &c, &d) != 4 ||
        a < 0 || a > 255 || b < 0 || b > 255 || 
        c < 0 || c > 255 || d < 0 || d > 255) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                          "Invalid IPv4 address format: %s", arg);
    }

    option->value.as_string = arg;
    return CARGS_SUCCESS;
}

// Handler for log level (debug, info, warning, error)
int log_level_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    if (arg == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
                          "Log level is required");
    }

    // Convert string to lowercase for case-insensitive comparison
    char* level = strdup(arg);
    if (!level) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Memory allocation failed");
    }
    
    for (char* p = level; *p; ++p) {
        *p = tolower(*p);
    }

    int value;
    if (strcmp(level, "debug") == 0) value = LOG_DEBUG;
    else if (strcmp(level, "info") == 0) value = LOG_INFO;
    else if (strcmp(level, "warning") == 0 || strcmp(level, "warn") == 0) value = LOG_WARNING;
    else if (strcmp(level, "error") == 0) value = LOG_ERROR;
    else {
        free(level);
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Invalid log level: %s (valid values: debug, info, warning, error)", arg);
    }

    free(level);
    option->value.as_int = value;
    return CARGS_SUCCESS;
}

// Handler for time duration with units (e.g., "10s", "5m")
int duration_handler(cargs_t *cargs, cargs_option_t *option, char *arg)
{
    if (arg == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
                          "Duration is required");
    }

    // Parse the numeric part
    char* endptr;
    long value = strtol(arg, &endptr, 10);
    
    if (endptr == arg) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                          "Invalid duration format: %s", arg);
    }
    
    // Check for units and convert to seconds
    if (*endptr == '\0' || strcmp(endptr, "s") == 0) {
        // Seconds (no conversion needed)
    } else if (strcmp(endptr, "m") == 0) {
        // Minutes to seconds
        value *= 60;
    } else if (strcmp(endptr, "h") == 0) {
        // Hours to seconds
        value *= 3600;
    } else if (strcmp(endptr, "d") == 0) {
        // Days to seconds
        value *= 86400;
    } else {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_FORMAT,
                          "Unknown time unit: %s (valid units: s, m, h, d)", endptr);
    }
    
    option->value.as_int = (int)value;
    return CARGS_SUCCESS;
}

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // IP address option with custom handler
    OPTION_BASE('i', "ip", "Server IP address", VALUE_TYPE_STRING, 
                HANDLER(ipv4_handler)),
    
    // Log level option with custom handler
    OPTION_BASE('l', "log-level", "Log level (debug, info, warning, error)", 
                VALUE_TYPE_INT, HANDLER(log_level_handler), 
                DEFAULT(1)),  // Default to INFO
    
    // Duration option with custom handler
    OPTION_BASE('t', "timeout", "Connection timeout with unit (e.g., 30s, 5m, 1h)", 
                VALUE_TYPE_INT, HANDLER(duration_handler), 
                DEFAULT(60))  // Default to 60 seconds
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "custom_handlers_example", "1.0.0");
    cargs.description = "Example of custom handlers";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    if (cargs_is_set(cargs, "ip")) {
        const char* ip = cargs_get_value(cargs, "ip").as_string;
        printf("Server IP: %s\n", ip);
    }
    
    // Log level
    int log_level = cargs_get_value(cargs, "log-level").as_int;
    const char* log_level_str;
    switch (log_level) {
        case LOG_DEBUG: log_level_str = "DEBUG"; break;
        case LOG_INFO: log_level_str = "INFO"; break;
        case LOG_WARNING: log_level_str = "WARNING"; break;
        case LOG_ERROR: log_level_str = "ERROR"; break;
        default: log_level_str = "UNKNOWN";
    }
    printf("Log level: %s (%d)\n", log_level_str, log_level);
    
    // Timeout duration
    int timeout = cargs_get_value(cargs, "timeout").as_int;
    printf("Timeout: %d seconds", timeout);
    
    // Format as human-readable duration
    if (timeout >= 86400) {
        printf(" (%.1f days)\n", timeout / 86400.0);
    } else if (timeout >= 3600) {
        printf(" (%.1f hours)\n", timeout / 3600.0);
    } else if (timeout >= 60) {
        printf(" (%.1f minutes)\n", timeout / 60.0);
    } else {
        printf("\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
