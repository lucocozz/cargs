# Input Validation

cargs offers several validation mechanisms to ensure that user inputs are correct and meet expectations.

## Built-in Validators

### Range Validation (RANGE)

The `RANGE` validator checks that a numeric value falls within a specified range.

```c
OPTION_INT('p', "port", "Port number",
           RANGE(1, 65535),  // Port between 1 and 65535
           DEFAULT(8080))
```

### Regular Expression Validation (REGEX)

The `REGEX` validator checks that a string matches a regular expression pattern.

```c
OPTION_STRING('e', "email", "Email address",
              REGEX(CARGS_RE_EMAIL))  // Using a predefined pattern
```

cargs provides many predefined patterns in `cargs/regex.h`:

- `CARGS_RE_EMAIL` - Email addresses
- `CARGS_RE_IPV4` - IPv4 addresses
- `CARGS_RE_MAC` - MAC addresses
- `CARGS_RE_URL` - URLs
- `CARGS_RE_ISO_DATE` - ISO format dates (YYYY-MM-DD)
- `CARGS_RE_SEMVER` - Semantic versions

You can also define your own patterns:

```c
OPTION_STRING('i', "id", "Identifier",
              REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000")))
```

### Choice Validation (CHOICES)

The `CHOICES` validator checks that a value is one of the specified options.

```c
OPTION_STRING('l', "level", "Logging level",
              CHOICES_STRING("debug", "info", "warning", "error"),
              DEFAULT("info"))
```

For different types:

```c
// Integer choices
OPTION_INT('m', "mode", "Mode",
           CHOICES_INT(1, 2, 3),
           DEFAULT(1))

// Float choices
OPTION_FLOAT('s', "scale", "Scale",
             CHOICES_FLOAT(0.5, 1.0, 2.0),
             DEFAULT(1.0))
```

## Custom Validators

You can create custom validators for more complex validation logic.

### Defining a Custom Validator

```c
// Validator for even numbers
int even_validator(cargs_t *cargs, value_t value, validator_data_t data)
{
    if (value.as_int % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                         "Value must be an even number");
    }
    return CARGS_SUCCESS;
}
```

### Using a Custom Validator

```c
OPTION_BASE('n', "number", "Even number", VALUE_TYPE_INT,
            HANDLER(int_handler),
            VALIDATOR(even_validator, NULL))
```

## Combining Validators

You can combine multiple validators for an option:

```c
OPTION_INT('p', "port", "Port number", 
          RANGE(1, 65535),                   // Range validator
          VALIDATOR(even_validator, NULL),   // Custom validator
          DEFAULT(8080))
```

## Pre-validators

Pre-validators are executed before the option is converted to its final type, operating on the raw string.

```c
// Pre-validator to check string length
int length_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
    size_t min_length = *((size_t *)data.custom);
    
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

## Validation Error Handling

When a validator fails, cargs:

1. Displays an explanatory error message
2. Returns a non-zero error code from `cargs_parse()`
3. Does not set the option with the invalid value

Example error message:

```
my_program: Value '70000' is outside the range [1, 65535] for option 'port'
```

## Validation with REGEX and Predefined Patterns

cargs uses PCRE2 for powerful regular expression validation.

```c
#include "cargs/regex.h"

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Standard email validation
    OPTION_STRING('e', "email", "Email address",
                REGEX(CARGS_RE_EMAIL)),
    
    // Strict email validation
    OPTION_STRING('E', "strict-email", "Email address (strict validation)",
                REGEX(CARGS_RE_EMAIL_STRICT)),
    
    // IP address validation
    OPTION_STRING('i', "ip", "IP address",
                REGEX(CARGS_RE_IPV4)),
    
    // URL validation
    OPTION_STRING('u', "url", "URL",
                REGEX(CARGS_RE_URL))
)
```

See [`includes/cargs/regex.h`](https://github.com/lucocozz/cargs/blob/main/includes/cargs/regex.h) for the complete list of predefined patterns.
