# Validation

cargs offers robust validation capabilities to ensure that command-line inputs meet your application's requirements.

!!! abstract "Overview"
    Validation in cargs is organized into several categories:
    
    - **Built-in Validators** - Simple validation with `RANGE` and `CHOICES`
    - **Regular Expression Validation** - String pattern validation with PCRE2
    - **Custom Validators** - Create your own validation logic
    
    This guide covers the basics of each approach. For advanced usage, see the specialized guides linked throughout.

## Built-in Validators

cargs provides several built-in validators to simplify common validation scenarios. These can be applied directly to option definitions.

### Range Validation

The `RANGE` validator ensures numeric values fall within a specified range:

=== "Definition"
    ```c
    OPTION_INT('p', "port", "Port number",
               RANGE(1, 65535),  // Must be between 1 and 65535
               DEFAULT(8080))
    ```

=== "Usage Example"
    ```bash
    $ ./my_program --port=9000  # Valid
    $ ./my_program --port=100000  # Error: Value 100000 is out of range [1, 65535]
    ```

### Choices Validation

The `CHOICES` validator ensures the value is one of a specific set:

=== "String Choices"
    ```c
    OPTION_STRING('l', "level", "Log level",
                  CHOICES_STRING("debug", "info", "warning", "error"),
                  DEFAULT("info"))
    ```

=== "Integer Choices"
    ```c
    OPTION_INT('m', "mode", "Operating mode",
               CHOICES_INT(1, 2, 3),
               DEFAULT(1))
    ```

=== "Float Choices"
    ```c
    OPTION_FLOAT('s', "scale", "Scale factor",
                CHOICES_FLOAT(0.5, 1.0, 2.0),
                DEFAULT(1.0))
    ```

## Regular Expression Validation

cargs uses PCRE2 for powerful regular expression validation:

=== "Basic Usage"
    ```c
    OPTION_STRING('e', "email", "Email address",
                  REGEX(CARGS_RE_EMAIL))  // Must be a valid email
    ```

=== "Custom Pattern"
    ```c
    OPTION_STRING('i', "id", "Product ID",
                  REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000")))
    ```

!!! tip "Predefined Patterns"
    cargs includes many predefined patterns in `cargs/regex.h`:
    
    | Constant | Validates | Example |
    |----------|-----------|---------|
    | `CARGS_RE_EMAIL` | Email addresses | user@example.com |
    | `CARGS_RE_IPV4` | IPv4 addresses | 192.168.1.1 |
    | `CARGS_RE_URL` | URLs | https://example.com |
    | `CARGS_RE_ISO_DATE` | ISO format dates | 2023-01-31 |
    
    For a complete list, see the [Regular Expressions API reference](../api/regex.md).
    
    For advanced regex usage, see the [Regular Expressions guide](../advanced/regex.md).

## Custom Validators

For more complex validation logic, you can create your own validators:

=== "Simple Validator"
    ```c
    int even_validator(cargs_t *cargs, value_t value, validator_data_t data)
    {
        if (value.as_int % 2 != 0) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                             "Value must be an even number");
        }
        return CARGS_SUCCESS;
    }
    
    // Usage
    OPTION_INT('n', "number", "An even number", 
              VALIDATOR(even_validator, NULL))
    ```

=== "Basic Pre-Validator"
    ```c
    int length_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
    {
        size_t min_length = *(size_t *)data.custom;
        
        if (strlen(value) < min_length) {
            CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                              "String must be at least %zu characters long", min_length);
        }
        return CARGS_SUCCESS;
    }
    
    // Usage
    size_t min_length = 8;
    OPTION_STRING('p', "password", "Password",
                 PRE_VALIDATOR(length_pre_validator, &min_length))
    ```

!!! info "Validator Types"
    cargs supports two types of custom validators:
    
    1. **Validators** - Validate the **processed** value after conversion to its final type
    2. **Pre-Validators** - Validate the **raw string** before any processing
    
    For a detailed exploration of custom validators, including examples and best practices, 
    see the [Custom Validators guide](../advanced/custom-validators.md).

## Combining Validators

You can apply multiple validators to a single option for more comprehensive validation:

```c
OPTION_INT('p', "port", "Port number", 
          RANGE(1, 65535),                   // Range validator
          VALIDATOR(even_validator, NULL),   // Custom validator
          DEFAULT(8080))
```

## Error Reporting

Validators should use `CARGS_REPORT_ERROR` to provide clear error messages:

```c
CARGS_REPORT_ERROR(cargs, error_code, format_string, ...);
```

!!! example "Error Message Example"
    ```
    my_program: Value '70000' is outside the range [1, 65535] for option 'port'
    ```

Common error codes include:
- `CARGS_ERROR_INVALID_VALUE`: Value doesn't meet requirements
- `CARGS_ERROR_INVALID_RANGE`: Value outside allowed range
- `CARGS_ERROR_INVALID_FORMAT`: Value has incorrect format

## Complete Examples

For complete working examples of validation techniques:

!!! tip "Example Files"
    - `examples/validators.c` - Demonstrates all validation techniques
    - `examples/regex.c` - Focused on regular expression validation

## Additional Resources

For more in-depth coverage of validation topics, refer to these advanced guides:

- [Custom Validators](../advanced/custom-validators.md) - Creating custom validators with specialized logic
- [Regular Expressions](../advanced/regex.md) - Detailed guide to regex pattern validation
- [Predefined Regex Patterns](../api/regex_patterns.md) - List of predefined regex patterns in cargs
