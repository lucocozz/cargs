# Custom Validators

This guide explains how to create and use custom validators with cargs to implement specialized validation logic for command-line options.

## Overview

Validation is essential for ensuring that command-line inputs meet your application's requirements. While cargs provides built-in validators like `RANGE()` and `REGEX()`, custom validators allow you to implement application-specific validation logic.

In this guide, you'll learn about:

- The two types of validators: **validators** and **pre-validators**
- How to create and use custom validators
- Techniques for passing configuration data to validators
- Best practices for validator implementation

## Understanding Validator Types

Cargs supports two distinct types of custom validation functions, each with a specific purpose:

### Validators

**Purpose**: Check the **processed** value after type conversion

**When to use**: When validating based on the final data type (int, float, string, etc.)

**Function signature**:
```c
int validator_function(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
```

### Pre-validators

**Purpose**: Check the **raw string** before it's processed

**When to use**: When you need to:
- Validate string format before parsing attempts
- Perform complex string validation
- Prevent type conversion errors

**Function signature**:
```c
int pre_validator_function(cargs_t *cargs, const char *value, validator_data_t data);
```

## Creating Basic Validators

Let's start with simple examples of both validator types.

### Example: Even Number Validator

This validator ensures that integer options have even values:

```c
int even_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    // Not using custom data in this example
    UNUSED(data);
    
    if (option->value.as_int % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                         "Value must be an even number, got %d", option->value.as_int);
    }
    return CARGS_SUCCESS;
}
```

**Using the validator**:

```c
OPTION_INT('n', "number", HELP("An even number"), 
          VALIDATOR(even_validator, NULL))
```

### Example: String Length Pre-validator

This pre-validator checks if a string meets a minimum length requirement:

```c
int string_length_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
    // Get minimum length from the validator data
    size_t min_length = *(size_t *)data.custom;
    
    if (strlen(value) < min_length) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "String must be at least %zu characters long", min_length);
    }
    return CARGS_SUCCESS;
}
```

**Using the pre-validator**:

```c
// Define the validation constraint
size_t min_length = 8;

OPTION_STRING('p', "password", HELP("Password"),
             PRE_VALIDATOR(string_length_pre_validator, &min_length))
```

## Passing Data to Validators

The `validator_data_t` parameter allows you to pass configuration data to your validators, making them more flexible and reusable.

### Using Custom Data Structures

For complex validation rules, you can create a structure to hold multiple parameters:

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_odd;
} number_constraints_t;

int number_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    // Get constraints from validator data
    number_constraints_t *constraints = (number_constraints_t *)data.custom;
    
    // Range validation
    if (option->value.as_int < constraints->min_value || 
        option->value.as_int > constraints->max_value) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE,
                          "Value must be between %d and %d", 
                          constraints->min_value, constraints->max_value);
    }
    
    // Even/odd validation
    if (!constraints->allow_odd && (option->value.as_int % 2 != 0)) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return CARGS_SUCCESS;
}
```

**Using the validator with custom data**:

```c
// Define constraints
static number_constraints_t constraints = {
    .min_value = 10,
    .max_value = 100,
    .allow_odd = false
};

OPTION_INT('n', "number", HELP("A number with constraints"), 
          VALIDATOR(number_validator, &constraints))
```

### Using Inline Compound Literals

For simple cases, you can use C99 compound literals to pass data inline:

```c
OPTION_STRING('u', "username", HELP("Username"),
             PRE_VALIDATOR(string_length_pre_validator, &((size_t){3})))
```

This creates an anonymous `size_t` variable with value 3 and passes its address to the validator.

## Advanced Validation Techniques

### Context-Aware Validation

Sometimes validators need to check values in relation to other options:

```c
typedef struct {
    const char *related_option;
} option_relation_t;

int greater_than_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    option_relation_t *relation = (option_relation_t *)data.custom;
    cargs_value_t other_value = cargs_get(*cargs, relation->related_option);
    
    if (option->value.as_int <= other_value.as_int) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be greater than '%s' (%d)",
                          relation->related_option, other_value.as_int);
    }
    
    return CARGS_SUCCESS;
}
```

**Usage example**:
```c
static option_relation_t max_relation = { .related_option = "min" };

CARGS_OPTIONS(
    options,
    OPTION_INT('n', "min", HELP("Minimum value")),
    OPTION_INT('x', "max", HELP("Maximum value"), 
               VALIDATOR(greater_than_validator, &max_relation))
)
```

### Creating Helper Macros

For frequently used validation patterns, create helper macros:

```c
// Helper macro for even number validation
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)

// Helper macro for minimum string length
#define MIN_LENGTH(min) \
    PRE_VALIDATOR(string_length_pre_validator, &((size_t){min}))

// Helper macro for maximum string length
#define MAX_LENGTH(max) \
    PRE_VALIDATOR(string_length_max_validator, &((size_t){max}))

// Combined length check
#define STRING_LENGTH(min, max) \
    PRE_VALIDATOR(string_length_range_validator, &((length_range_t){min, max}))
```

**Usage example**:
```c
CARGS_OPTIONS(
    options,
    OPTION_INT('n', "number", HELP("An even number"), EVEN_NUMBER()),
    OPTION_STRING('p', "password", HELP("Password"), MIN_LENGTH(8)),
    OPTION_STRING('u', "username", HELP("Username"), STRING_LENGTH(3, 20))
)
```

## Combining Multiple Validators

Cargs allows you to apply multiple validators to a single option by using the numbered validator macros:

```c
OPTION_INT('p', "port", HELP("Port number"), 
          VALIDATOR(is_even_validator, NULL),      // First validator
          VALIDATOR2(range_validator, &port_range), // Second validator
          VALIDATOR3(port_validator, NULL))        // Third validator
```

Cargs has a limit of 4 validators per option, but you can modify the constant `CARGS_MAX_VALIDATORS` to increase this limit.

Note that built-in validators like `RANGE()`, `LENGTH()`, and `COUNT()` use the first validator slot. You can combine them with custom validators by using the second and subsequent slots:

```c
OPTION_INT('p', "port", HELP("Port number"),
          RANGE(1, 65535),                     // Uses first validator slot
          VALIDATOR2(is_even_validator, NULL)) // Uses second validator slot
```

## Error Reporting

Validators should use `CARGS_REPORT_ERROR` to provide clear error messages:

```c
CARGS_REPORT_ERROR(cargs, error_code, format_string, ...);
```

**Common error codes**:

| Error Code | Description | Typical Use |
|------------|-------------|-------------|
| `CARGS_ERROR_INVALID_VALUE` | Value doesn't meet requirements | General validation failures |
| `CARGS_ERROR_INVALID_RANGE` | Value outside allowed range | Range validation |
| `CARGS_ERROR_INVALID_FORMAT` | Value has incorrect format | Format validation |
| `CARGS_ERROR_MEMORY` | Memory allocation failed | During validation processing |

## Best Practices

### 1. Single Responsibility

Each validator should focus on one validation concern:

```c
// Good: Two focused validators
int is_even_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int in_range_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);

// Use them together
OPTION_INT('n', "number", HELP("Number"), 
          VALIDATOR(is_even_validator, NULL),
          VALIDATOR2(in_range_validator, &range))
```

### 2. Descriptive Error Messages

Provide clear, actionable error messages:

```c
// Good: Clear and specific message
CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                  "Username must be 3-20 characters with only letters, numbers, and underscores");

// Bad: Vague message
CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Invalid input");
```

### 3. Parameter Safety

Always validate parameters and handle edge cases:

```c
int string_length_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    // Check if value is NULL before using it
    if (option->value.as_string == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "String cannot be NULL");
    }
    
    // Rest of the validation logic...
    return CARGS_SUCCESS;
}
```

### 4. Memory Efficiency

Avoid unnecessary heap allocations in validators:

```c
int efficient_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    // Use stack-based buffers for temporary operations
    char buffer[256];
    
    // Process value without heap allocations
    
    return CARGS_SUCCESS;
}
```

### 5. Reusable Components

Design validators to be reusable across options:

```c
// Generic validator for checking if a number is divisible by n
int divisible_by_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data)
{
    int divisor = *(int *)data.custom;
    
    if (option->value.as_int % divisor != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    
    return CARGS_SUCCESS;
}

// Reuse with different configurations
OPTION_INT('n', "number", HELP("Number divisible by 2"), 
          VALIDATOR(divisible_by_validator, &((int){2})));

OPTION_INT('m', "multiple", HELP("Multiple of 5"), 
          VALIDATOR(divisible_by_validator, &((int){5})));
```

## Complete Example

Here's a complete example demonstrating various custom validator techniques:

```c
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

// Custom pre-validator for string case requirements
int case_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
    typedef enum { LOWERCASE, UPPERCASE, MIXED } case_requirement_t;
    case_requirement_t req = *(case_requirement_t *)data.custom;
    
    bool has_upper = false;
    bool has_lower = false;
    
    for (const char *p = value; *p; p++) {
        if (isupper(*p)) has_upper = true;
        if (islower(*p)) has_lower = true;
    }
    
    switch (req) {
        case LOWERCASE:
            if (has_upper) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "Value must be lowercase only");
            }
            break;
            
        case UPPERCASE:
            if (has_lower) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "Value must be uppercase only");
            }
            break;
            
        case MIXED:
            if (!has_upper || !has_lower) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "Value must contain both uppercase and lowercase letters");
            }
            break;
    }
    
    return CARGS_SUCCESS;
}

// Helper macros for common validations
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)
#define EMAIL_VALIDATOR() VALIDATOR(email_validator, NULL)
#define LOWERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){LOWERCASE}))
#define UPPERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){UPPERCASE}))
#define MIXED_CASE() PRE_VALIDATOR(case_pre_validator, &((int){MIXED}))

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
    
    // Custom email validator
    OPTION_STRING('e', "email", HELP("Email address"),
                EMAIL_VALIDATOR()),
    
    // Custom even number validator
    OPTION_INT('n', "number", HELP("An even number"),
                EVEN_NUMBER(),
                DEFAULT(42)),
    
    // String with case validation
    OPTION_STRING('u', "username", HELP("Username (lowercase)"),
                LOWERCASE_ONLY()),
                
    // String with multiple validators
    OPTION_STRING('p', "password", HELP("Password (mixed case)"),
                MIXED_CASE())
)

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "validators_example", "1.0.0");
    cargs.description = "Example of custom validators";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    int port = cargs_get(cargs, "port").as_int;
    const char* log_level = cargs_get(cargs, "log-level").as_string;
    int number = cargs_get(cargs, "number").as_int;
    
    const char* email = cargs_is_set(cargs, "email") ? 
                        cargs_get(cargs, "email").as_string : "not set";
    
    const char* username = cargs_is_set(cargs, "username") ?
                          cargs_get(cargs, "username").as_string : "not set";
    
    const char* password = cargs_is_set(cargs, "password") ?
                          cargs_get(cargs, "password").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (range: 1-65535)\n", port);
    printf("  Log level: %s (choices: debug, info, warning, error)\n", log_level);
    printf("  Even number: %d (must be even)\n", number);
    printf("  Email: %s (must be valid email format)\n", email);
    printf("  Username: %s (must be lowercase)\n", username);
    printf("  Password: %s (must contain mixed case)\n", password);
    
    cargs_free(&cargs);
    return 0;
}
```

## Related Documentation

- [Validation Guide](../guide/validation.md) - Basic validation concepts
- [Regular Expressions Guide](regex.md) - Validation with regex patterns
