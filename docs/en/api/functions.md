# Functions Reference

This page provides a comprehensive reference for all the functions in the cargs API. These functions allow you to initialize the library, parse arguments, access values, and manage resources.

!!! abstract "Overview"
    The cargs API is organized into several function groups:
    
    - **Initialization and Parsing** - Core functions for setup and parsing
    - **Value Access** - Functions for retrieving option values
    - **Collection Access** - Functions for working with arrays and maps
    - **Subcommand Management** - Functions for handling subcommands
    - **Display Functions** - Functions for generating help and usage text
    - **Error Handling** - Functions for managing and reporting errors

## Initialization and Parsing

### cargs_init

Initializes a cargs context with program information and options.

```c
cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version);
```

**Parameters:**
- `options`: Array of command-line options defined with `CARGS_OPTIONS`
- `program_name`: Name of the program (used in help/error messages)
- `version`: Version string

**Returns:**
- An initialized `cargs_t` structure

**Example:**
```c
cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
cargs.description = "My awesome program description";
```

!!! tip
    After initialization, you can set additional fields like `description` and `env_prefix` before parsing.

### cargs_parse

Parses command-line arguments according to the defined options.

```c
int cargs_parse(cargs_t *cargs, int argc, char **argv);
```

**Parameters:**
- `cargs`: Pointer to the initialized cargs context
- `argc`: Argument count (from `main`)
- `argv`: Argument values (from `main`)

**Returns:**
- `CARGS_SUCCESS` (0) on success
- A non-zero error code on failure

**Example:**
```c
int status = cargs_parse(&cargs, argc, argv);
if (status != CARGS_SUCCESS) {
    return status;
}
```

### cargs_free

Frees resources allocated during parsing.

```c
void cargs_free(cargs_t *cargs);
```

**Parameters:**
- `cargs`: Pointer to the cargs context to free

**Example:**
```c
cargs_free(&cargs);
```

!!! warning
    Always call `cargs_free()` when you're done with a cargs context to avoid memory leaks.

## Value Access

### cargs_get

Retrieves the value of an option.

```c
value_t cargs_get(cargs_t cargs, const char *option_path);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- The option's value as a `value_t` union, or `{.raw = 0}` if not found

**Example:**
```c
const char *output = cargs_get(cargs, "output").as_string;
int port = cargs_get(cargs, "port").as_int;
bool verbose = cargs_get(cargs, "verbose").as_bool;
```

### cargs_is_set

Checks if an option was explicitly set on the command line.

```c
bool cargs_is_set(cargs_t cargs, const char *option_path);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- `true` if the option was set, `false` otherwise

**Example:**
```c
if (cargs_is_set(cargs, "verbose")) {
    // Verbose mode enabled
}
```

### cargs_count

Gets the number of values for an option (for collections).

```c
size_t cargs_count(cargs_t cargs, const char *option_path);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the option (name or `subcommand.name` format)

**Returns:**
- Number of values for the option, or 0 if not found or not a collection

**Example:**
```c
size_t tags_count = cargs_count(cargs, "tags");
printf("Tags: %zu\n", tags_count);
```

## Collection Access

### cargs_array_get

Retrieves an element from an array option at the specified index.

```c
value_t cargs_array_get(cargs_t cargs, const char *option_path, size_t index);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the array option
- `index`: Index of the element to retrieve

**Returns:**
- The value at the specified index, or `{.raw = 0}` if not found or index out of bounds

**Example:**
```c
const char *first_tag = cargs_array_get(cargs, "tags", 0).as_string;
```

### cargs_map_get

Retrieves a value from a map option with the specified key.

```c
value_t cargs_map_get(cargs_t cargs, const char *option_path, const char *key);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the map option
- `key`: Key to look up in the map

**Returns:**
- The value associated with the key, or `{.raw = 0}` if not found

**Example:**
```c
const char *user = cargs_map_get(cargs, "env", "USER").as_string;
int http_port = cargs_map_get(cargs, "ports", "http").as_int;
```

### cargs_array_it

Creates an iterator for efficiently traversing an array option.

```c
cargs_array_it_t cargs_array_it(cargs_t cargs, const char *option_path);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the array option

**Returns:**
- Iterator structure for the array, with `count=0` if option not found

**Example:**
```c
cargs_array_it_t it = cargs_array_it(cargs, "tags");
```

### cargs_array_next

Gets the next element from an array iterator.

```c
bool cargs_array_next(cargs_array_it_t *it);
```

**Parameters:**
- `it`: Array iterator

**Returns:**
- `true` if a value was retrieved, `false` if end of array

**Example:**
```c
cargs_array_it_t it = cargs_array_it(cargs, "tags");
while (cargs_array_next(&it)) {
    printf("Tag: %s\n", it.value.as_string);
}
```

### cargs_array_reset

Resets an array iterator to the beginning.

```c
void cargs_array_reset(cargs_array_it_t *it);
```

**Parameters:**
- `it`: Array iterator to reset

**Example:**
```c
cargs_array_reset(&it);  // Reset to start a new iteration
```

### cargs_map_it

Creates an iterator for efficiently traversing a map option.

```c
cargs_map_it_t cargs_map_it(cargs_t cargs, const char *option_path);
```

**Parameters:**
- `cargs`: The cargs context
- `option_path`: Path to the map option

**Returns:**
- Iterator structure for the map, with `count=0` if option not found

**Example:**
```c
cargs_map_it_t it = cargs_map_it(cargs, "env");
```

### cargs_map_next

Gets the next key-value pair from a map iterator.

```c
bool cargs_map_next(cargs_map_it_t *it);
```

**Parameters:**
- `it`: Map iterator

**Returns:**
- `true` if a pair was retrieved, `false` if end of map

**Example:**
```c
cargs_map_it_t it = cargs_map_it(cargs, "env");
while (cargs_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

### cargs_map_reset

Resets a map iterator to the beginning.

```c
void cargs_map_reset(cargs_map_it_t *it);
```

**Parameters:**
- `it`: Map iterator to reset

**Example:**
```c
cargs_map_reset(&it);  // Reset to start a new iteration
```

## Subcommand Management

### cargs_has_command

Checks if a subcommand was specified on the command line.

```c
bool cargs_has_command(cargs_t cargs);
```

**Parameters:**
- `cargs`: The cargs context

**Returns:**
- `true` if a subcommand was specified, `false` otherwise

**Example:**
```c
if (cargs_has_command(cargs)) {
    // A subcommand was specified
} else {
    printf("No command specified. Use --help to see available commands.\n");
}
```

### cargs_exec

Executes the action associated with the specified subcommand.

```c
int cargs_exec(cargs_t *cargs, void *data);
```

**Parameters:**
- `cargs`: Pointer to the cargs context
- `data`: Optional data to pass to the subcommand action

**Returns:**
- Status code returned by the subcommand action
- `CARGS_ERROR_NO_COMMAND` if no command was specified
- `CARGS_ERROR_INVALID_HANDLER` if the command has no action

**Example:**
```c
if (cargs_has_command(cargs)) {
    status = cargs_exec(&cargs, NULL);
}
```

## Display Functions

### cargs_print_help

Prints a formatted help message based on the defined options.

```c
void cargs_print_help(cargs_t cargs);
```

**Parameters:**
- `cargs`: The cargs context

**Example:**
```c
cargs_print_help(cargs);
```

### cargs_print_usage

Prints a short usage summary.

```c
void cargs_print_usage(cargs_t cargs);
```

**Parameters:**
- `cargs`: The cargs context

**Example:**
```c
cargs_print_usage(cargs);
```

### cargs_print_version

Prints version information.

```c
void cargs_print_version(cargs_t cargs);
```

**Parameters:**
- `cargs`: The cargs context

**Example:**
```c
cargs_print_version(cargs);
```

## Error Handling

### cargs_print_error_stack

Prints all errors in the error stack.

```c
void cargs_print_error_stack(const cargs_t *cargs);
```

**Parameters:**
- `cargs`: The cargs context

**Example:**
```c
if (status != CARGS_SUCCESS) {
    cargs_print_error_stack(&cargs);
    return status;
}
```

### cargs_strerror

Gets a string description of an error code.

```c
const char *cargs_strerror(cargs_error_type_t error);
```

**Parameters:**
- `error`: Error code

**Returns:**
- String description of the error

**Example:**
```c
printf("Error: %s\n", cargs_strerror(status));
```

## Complete Example

Here's a complete example showing the main function usage pattern:

```c
#include "cargs.h"
#include <stdio.h>

// Define options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
    POSITIONAL_STRING("input", "Input file")
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "example", "1.0.0");
    cargs.description = "Example program using cargs";
    
    // Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        cargs_print_error_stack(&cargs);
        return status;
    }
    
    // Access values
    const char *input = cargs_get(cargs, "input").as_string;
    const char *output = cargs_get(cargs, "output").as_string;
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    
    // Application logic
    if (verbose) {
        printf("Input: %s\n", input);
        printf("Output: %s\n", output);
    }
    
    // Free resources
    cargs_free(&cargs);
    return 0;
}
```

## Function Categories

For ease of reference, here's a summary of the function categories:

| Category | Functions |
|----------|-----------|
| **Initialization** | `cargs_init`, `cargs_parse`, `cargs_free` |
| **Value Access** | `cargs_get`, `cargs_is_set`, `cargs_count` |
| **Array Functions** | `cargs_array_get`, `cargs_array_it`, `cargs_array_next`, `cargs_array_reset` |
| **Map Functions** | `cargs_map_get`, `cargs_map_it`, `cargs_map_next`, `cargs_map_reset` |
| **Subcommand Functions** | `cargs_has_command`, `cargs_exec` |
| **Display Functions** | `cargs_print_help`, `cargs_print_usage`, `cargs_print_version` |
| **Error Functions** | `cargs_print_error_stack`, `cargs_strerror` |

## Related Documentation

- [Types Reference](types.md) - Detailed information about data types
- [Macros Reference](macros.md) - Complete list of option definition macros
- [API Overview](overview.md) - High-level overview of the cargs API
