# Custom Validators

Custom validators extend cargs' capabilities by allowing you to implement specialized validation logic for command-line options.

!!! abstract "Overview"
    This guide covers advanced validation techniques with custom validators:
    
    - **Validators** and **Pre-validators** - Two types of custom validation functions
    - **Data-driven validation** - Passing configuration to validators
    - **Helper macros** - Creating reusable validation components
    - **Best practices** - Guidelines for effective validator implementation
    
    For basic validation concepts, see the [Validation guide](../guide/validation.md).

## Validator Types

cargs supports two types of custom validation functions:

### 1. Validators

Validators check the **processed** value after the handler has converted it to its final type:

```c
int validator_function(cargs_t *cargs, cargs_value_t value, validator_data_t data);
```

Use validators when you need to validate based on the processed value's type (int, float, etc.).

### 2. Pre-validators

Pre-validators check the **raw string** before it's processed by the handler:

```c
int pre_validator_function(cargs_t *cargs, const char *value, validator_data_t data);
```

Use pre-validators when you need to:
- Examine raw string format before parsing
- Perform complex string validation
- Validate before attempting conversion

## Creating Custom Validators

### Basic Validator

Let's create a simple validator that ensures an integer is even:

```c
int even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    UNUSED(data);  // Not using custom data in this example
    
    if (value.as_int % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                         "Value must be an even number, got %d", value.as_int);
    }
    return CARGS_SUCCESS;
}
```

To use this validator:

```c
OPTION_INT('n', "number", "An even number", 
          VALIDATOR(even_validator, NULL))
```

### Basic Pre-validator

Here's a pre-validator that checks string length before processing:

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

To use this pre-validator:

```c
// Define the validation constraint
size_t min_length = 8;

OPTION_STRING('p', "password", "Password", 
            PRE_VALIDATOR(string_length_pre_validator, &min_length))
```

## Passing Data to Validators

The `validator_data_t` parameter allows you to pass custom data to your validators for more flexible validation.

### Using Custom Data Structures

Create a structure to hold validation parameters:

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_odd;
} number_constraints_t;
```

Create a validator that uses this structure:

```c
int custom_number_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Get constraints from validator data
    number_constraints_t *constraints = (number_constraints_t *)data.custom;
    
    // Validate range
    if (value.as_int < constraints->min_value || value.as_int > constraints->max_value) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE,
                          "Value must be between %d and %d", 
                          constraints->min_value, constraints->max_value);
    }
    
    // Validate odd/even if required
    if (!constraints->allow_odd && (value.as_int % 2 != 0)) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return CARGS_SUCCESS;
}
```

Use the validator with custom data:

```c
// Define constraints
static number_constraints_t constraints = {
    .min_value = 10,
    .max_value = 100,
    .allow_odd = false
};

OPTION_INT('n', "number", "A number with constraints", 
          VALIDATOR(custom_number_validator, &constraints))
```

### Using Static Configuration

For simple configurations, you can use static variables:

```c
// Static configuration for string length validator
static size_t USERNAME_MIN_LENGTH = 3;
static size_t USERNAME_MAX_LENGTH = 20;

int username_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    UNUSED(data);
    const char *username = value.as_string;
    
    if (username == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Username cannot be NULL");
    }
    
    size_t length = strlen(username);
    if (length < USERNAME_MIN_LENGTH || length > USERNAME_MAX_LENGTH) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Username must be between %zu and %zu characters",
                          USERNAME_MIN_LENGTH, USERNAME_MAX_LENGTH);
    }
    
    return CARGS_SUCCESS;
}
```

## Advanced Validation Techniques

### Inline Variables with Compound Literals

You can pass data without a separate variable using compound literals:

```c
OPTION_STRING('u', "username", "Username",
             PRE_VALIDATOR(string_length_pre_validator, &((size_t){3})))
```

This creates an anonymous `size_t` variable with value 3 and passes its address to the validator.

### Context-Aware Validation

Sometimes validators need to check values in relation to other options:

```c
typedef struct {
    const char *related_option;
} option_relation_t;

int greater_than_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    option_relation_t *relation = (option_relation_t *)data.custom;
    cargs_value_t other_value = cargs_get(*cargs, relation->related_option);
    
    if (value.as_int <= other_value.as_int) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be greater than '%s' (%d)",
                          relation->related_option, other_value.as_int);
    }
    
    return CARGS_SUCCESS;
}

// Usage
static option_relation_t max_relation = { .related_option = "min" };

CARGS_OPTIONS(
    options,
    OPTION_INT('n', "min", "Minimum value"),
    OPTION_INT('x', "max", "Maximum value", 
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

// Usage
CARGS_OPTIONS(
    options,
    OPTION_INT('n', "number", "An even number", EVEN_NUMBER()),
    OPTION_STRING('p', "password", "Password", MIN_LENGTH(8)),
    OPTION_STRING('u', "username", "Username", STRING_LENGTH(3, 20))
)
```

## Error Reporting

Validators should use `CARGS_REPORT_ERROR` to report validation failures:

```c
CARGS_REPORT_ERROR(cargs, error_code, format_string, ...);
```

Common error codes:

| Error Code | Description | Typical Use |
|------------|-------------|-------------|
| `CARGS_ERROR_INVALID_VALUE` | Value doesn't meet requirements | General validation failures |
| `CARGS_ERROR_INVALID_RANGE` | Value outside allowed range | Range validation |
| `CARGS_ERROR_INVALID_FORMAT` | Value has incorrect format | Format validation |
| `CARGS_ERROR_MEMORY` | Memory allocation failed | During validation processing |

## Best Practices

### 1. Single Responsibility

Each validator should focus on one validation concern:

=== "Good: Focused Validators"
    ```c
    int is_even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data);
    int in_range_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data);
    
    // Use both validators together
    OPTION_INT('n', "number", "Number", 
              VALIDATOR(is_even_validator, NULL),
              VALIDATOR(in_range_validator, &range))
    ```

=== "Bad: Monolithic Validator"
    ```c
    int complex_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
    {
        // Does too many things in one function
        // - Checks if value is even
        // - Validates range
        // - Checks prime numbers
        // - Verifies special business rules
        // ...
    }
    ```

### 2. Descriptive Error Messages

Provide clear, actionable error messages:

=== "Good: Helpful Messages"
    ```c
    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                      "Username must be 3-20 characters and contain only letters, numbers, and underscores");
    ```

=== "Bad: Unclear Messages"
    ```c
    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Invalid input");
    ```

### 3. Parameter Safety

Always validate parameters and handle edge cases:

```c
int string_length_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Check if value is NULL
    if (value.as_string == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "String cannot be NULL");
    }
    
    // Rest of the validation logic...
    return CARGS_SUCCESS;
}
```

### 4. Memory Considerations

Avoid excessive allocations in validators:

```c
int efficient_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Use stack-based buffers for temporary operations
    char buffer[256];
    
    // Process value without unnecessary heap allocations
    
    return CARGS_SUCCESS;
}
```

### 5. Reusable Components

Design validators to be reusable across multiple options:

```c
// Generic validator for checking if a number is divisible by n
int divisible_by_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    int divisor = *(int *)data.custom;
    
    if (value.as_int % divisor != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    
    return CARGS_SUCCESS;
}

// Reusable across different options
OPTION_INT('n', "number", "Number divisible by 2", 
          VALIDATOR(divisible_by_validator, &((int){2})));

OPTION_INT('m', "multiple", "Multiple of 5", 
          VALIDATOR(divisible_by_validator, &((int){5})));
```

## Complete Example

Here's a comprehensive example demonstrating various custom validator techniques:

```c
#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Custom validator for email addresses
int email_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
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
int even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    (void)data; // Unused parameter
    
    int number = value.as_int;
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
    OPTION_INT('p', "port", "Port number", 
                DEFAULT(8080), RANGE(1, 65535)),
    
    // Built-in choices validator
    OPTION_STRING('l', "log-level", "Log level", 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Custom email validator
    OPTION_STRING('e', "email", "Email address",
                EMAIL_VALIDATOR()),
    
    // Custom even number validator
    OPTION_INT('n', "number", "An even number",
                EVEN_NUMBER(),
                DEFAULT(42)),
    
    // String with case validation
    OPTION_STRING('u', "username", "Username (lowercase)",
                LOWERCASE_ONLY()),
                
    // String with multiple validators
    OPTION_STRING('p', "password", "Password (mixed case)",
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
