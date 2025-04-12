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
int email_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    const char* email = option->value.as_string;
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
int even_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    int number = option->value.as_int;
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
    OPTION_INT('p', "port", HELP("Port number"), 
                DEFAULT(8080), RANGE(1, 65535)),
    
    // Built-in choices validator
    OPTION_STRING('l', "log-level", HELP("Log level"), 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Built-in length validator
    OPTION_STRING('u', "username", HELP("Username (3-16 chars)"),
                DEFAULT("user"), LENGTH(3, 16)),
                
    // Built-in count validator (for arrays and maps)
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags (1-5 allowed)"),
                COUNT(1, 5)),
    
    // Custom email validator
    OPTION_STRING('e', "email", HELP("Email address"),
                VALIDATOR(email_validator, NULL)),
    
    // Custom even number validator
    OPTION_INT('n', "number", HELP("An even number"),
                VALIDATOR(even_validator, NULL),
                DEFAULT(42))
)

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "validators_example", "1.0.0");
    cargs.description = "Example of validators";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    int port = cargs_get(cargs, "port").as_int;
    const char* log_level = cargs_get(cargs, "log-level").as_string;
    const char* username = cargs_get(cargs, "username").as_string;
    int number = cargs_get(cargs, "number").as_int;
    const char* email = cargs_is_set(cargs, "email") ? 
                        cargs_get(cargs, "email").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (range: 1-65535)\n", port);
    printf("  Log level: %s (choices: debug, info, warning, error)\n", log_level);
    printf("  Username: %s (length: 3-16)\n", username);
    printf("  Even number: %d (must be even)\n", number);
    printf("  Email: %s (must be valid email format)\n", email);
    
    if (cargs_is_set(cargs, "tags")) {
        size_t count = cargs_count(cargs, "tags");
        cargs_value_t *tags = cargs_get(cargs, "tags").as_array;
        printf("  Tags (%zu items, count: 1-5):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("    - %s\n", tags[i].as_string);
        }
    }
    
    cargs_free(&cargs);
    return 0;
}
