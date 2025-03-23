# API Overview

This page provides an overview of the architecture and main components of the cargs API.

## Architecture

cargs is organized around several key components:

1. **Option Definition** - Macros for defining options
2. **Argument Parsing** - Functions for processing arguments
3. **Value Access** - Functions for retrieving parsed values
4. **Validation** - Mechanisms for validating inputs
5. **Error Handling** - System for managing and reporting errors

## Key Components

### Main Structure

The `cargs_t` structure is the core of cargs, containing all necessary data:

```c
typedef struct cargs_s {
    /* Public fields */
    const char *program_name;    // Program name
    const char *version;         // Program version
    const char *description;     // Program description
    const char *env_prefix;      // Prefix for environment variables
    
    /* Internal fields - do not access directly */
    cargs_option_t     *options;      // Defined options
    cargs_error_stack_t error_stack;  // Error stack
    // Other internal fields...
} cargs_t;
```

### Option Definition

Options are defined using macros:

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt"))
    // ...
)
```

The `cargs_option_t` structure represents an individual option.

### Value Types

Option values are stored in a `value_t` union that can contain different types:

```c
union value_u {
    uintptr_t raw;
    void     *as_ptr;
    char     *as_string;
    int       as_int;
    double    as_float;
    bool      as_bool;
    
    // Types for collections
    value_t      *as_array;
    cargs_pair_t *as_map;
    // ...
};
```

Supported types are enumerated in `value_type_t`.

## Lifecycle

Typical cargs usage follows this lifecycle:

1. **Define options** with `CARGS_OPTIONS`
2. **Initialize** with `cargs_init()`
3. **Parse arguments** with `cargs_parse()`
4. **Access values** with `cargs_get()`, `cargs_is_set()`, etc.
5. **Free resources** with `cargs_free()`

```c
// 1. Define options
CARGS_OPTIONS(options, /* ... */)

int main(int argc, char **argv)
{
    // 2. Initialize
    cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
    
    // 3. Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // 4. Access values
    const char *output = cargs_get(cargs, "output").as_string;
    
    // Application logic...
    
    // 5. Free resources
    cargs_free(&cargs);
    return 0;
}
```

## Function Families

The cargs API is organized into several function families:

### Initialization and Parsing Functions

- `cargs_init()` - Initializes the cargs context
- `cargs_parse()` - Parses command-line arguments
- `cargs_free()` - Frees resources

### Value Access Functions

- `cargs_get()` - Retrieves an option's value
- `cargs_is_set()` - Checks if an option was set
- `cargs_count()` - Gets the number of values for an option
- `cargs_array_get()` - Retrieves an element from an array
- `cargs_map_get()` - Retrieves a value from a map

### Iteration Functions

- `cargs_array_it()`, `cargs_array_next()`, `cargs_array_reset()` - Array iteration
- `cargs_map_it()`, `cargs_map_next()`, `cargs_map_reset()` - Map iteration

### Subcommand Functions

- `cargs_has_command()` - Checks if a subcommand was specified
- `cargs_exec()` - Executes the action associated with a subcommand

### Display Functions

- `cargs_print_help()` - Displays a formatted help message
- `cargs_print_usage()` - Displays a short usage message
- `cargs_print_version()` - Displays version information

## Error Handling

cargs uses an error code system and an error stack for error handling:

```c
// Parse arguments with error handling
int status = cargs_parse(&cargs, argc, argv);
if (status != CARGS_SUCCESS) {
    // An error occurred during parsing
    cargs_print_error_stack(&cargs);  // Display errors
    return status;
}
```

Error codes are defined in `cargs_error_type_t`.

## Advanced Components

### Validators

Validators check that input values meet certain criteria:

```c
// Range validator
OPTION_INT('p', "port", "Port number", RANGE(1, 65535))

// Regular expression validator
OPTION_STRING('e', "email", "Email address", REGEX(CARGS_RE_EMAIL))

// Custom validator
OPTION_BASE('n', "number", "Even number", VALUE_TYPE_INT,
            HANDLER(int_handler),
            VALIDATOR(even_validator, NULL))
```

### Handlers

Handlers process input values and convert them to internal representations:

```c
// Custom handler
int ipv4_handler(cargs_t *cargs, cargs_option_t *option, char *arg);

// Usage
OPTION_BASE('i', "ip", "IP address", VALUE_TYPE_STRING,
            HANDLER(ipv4_handler))
```

## Header File Organization

The cargs public API is distributed across several header files:

- `cargs.h` - Main entry point, includes all other headers
- `cargs/types.h` - Type and data structure definitions
- `cargs/options.h` - Macros for defining options
- `cargs/api.h` - Public API functions
- `cargs/errors.h` - Error codes and error handling functions
- `cargs/regex.h` - Predefined regular expression patterns
