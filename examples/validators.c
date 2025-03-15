/**
 * Validators example for cargs
 * 
 * Demonstrates built-in validators and custom validators
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Custom validator for email addresses
int email_validator(cargs_t *cargs, value_t value, validator_data_t data) {
    (void)data; // Unused parameter
    
    const char* email = value.as_string;
    if (!email) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Email address cannot be NULL");
    }
    
    // Check for @ character
    const char* at = strchr(email, '@');
    if (!at) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Email address must contain an '@' character");
    }
    
    // Check for domain
    const char* dot = strchr(at, '.');
    if (!dot) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Email domain must contain a '.' character");
    }
    
    return CARGS_SUCCESS;
}

// Custom validator for even numbers
int even_validator(cargs_t *cargs, value_t value, validator_data_t data) {
    (void)data; // Unused parameter
    
    int number = value.as_int;
    if (number % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return CARGS_SUCCESS;
}

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Built-in range validator
    OPTION_INT('p', "port", "Port number", 
               DEFAULT(8080), RANGE(1, 65535)),
    
    // Built-in choices validator
    OPTION_STRING('l', "log-level", "Log level", 
                 DEFAULT("info"), 
                 CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Custom email validator
    OPTION_BASE('e', "email", "Email address", VALUE_TYPE_STRING,
                HANDLER(string_handler), 
                VALIDATOR(email_validator, NULL, 0)),
    
    // Custom even number validator
    OPTION_BASE('n', "number", "An even number", VALUE_TYPE_INT,
               HANDLER(int_handler), 
               VALIDATOR(even_validator, NULL, 0),
               DEFAULT(42))
)

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "validators_example", "1.0.0",
                             "Example of validators");
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    int port = cargs_get_value(cargs, "port").as_int;
    const char* log_level = cargs_get_value(cargs, "log-level").as_string;
    int number = cargs_get_value(cargs, "number").as_int;
    const char* email = cargs_is_set(cargs, "email") ? 
                        cargs_get_value(cargs, "email").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (range: 1-65535)\n", port);
    printf("  Log level: %s (choices: debug, info, warning, error)\n", log_level);
    printf("  Even number: %d (must be even)\n", number);
    printf("  Email: %s (must be valid email format)\n", email);
    
    cargs_free(&cargs);
    return 0;
}
