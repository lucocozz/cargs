# Quick Start

This guide will help you quickly create a simple application using cargs to process command-line arguments.

## Minimal Example

Here's a minimal example of using cargs:

```c
#include "cargs.h"
#include <stdio.h>

// Define options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose mode")
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
    
    // Parse arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    
    // Application logic
    if (verbose) {
        printf("Verbose mode enabled\n");
    }
    
    // Free resources
    cargs_free(&cargs);
    return 0;
}
```

## Fundamental Steps

!!! abstract "Process"
    Here are the essential steps to use cargs in your application:

    1. **Include the header**: Start by including the `cargs.h` header.
    2. **Define options**: Use the `CARGS_OPTIONS` macro to define the options accepted by your program.
    3. **Initialize cargs**: Call `cargs_init()` to initialize the cargs context.
    4. **Parse arguments**: Use `cargs_parse()` to parse the command-line arguments.
    5. **Access values**: Use `cargs_get()`, `cargs_is_set()` and other functions to access option values.
    6. **Free resources**: Call `cargs_free()` before exiting to release allocated resources.

## Option Types

Cargs supports several types of options that you can use in your application:

| Type | Macro | Description | Example |
|------|-------|-------------|---------|
| **Flag** | `OPTION_FLAG` | Boolean option (presence/absence) | `OPTION_FLAG('v', "verbose", "Verbose mode")` |
| **Boolean** | `OPTION_BOOL` | Boolean option with true/false value | `OPTION_BOOL('d', "debug", "Debug mode")` |
| **String** | `OPTION_STRING` | Option with string value | `OPTION_STRING('o', "output", "Output file")` |
| **Integer** | `OPTION_INT` | Option with integer value | `OPTION_INT('p', "port", "Port number")` |
| **Float** | `OPTION_FLOAT` | Option with floating-point value | `OPTION_FLOAT('f', "factor", "Scale factor")` |
| **Positional** | `POSITIONAL_*` | Positional argument | `POSITIONAL_STRING("input", "Input file")` |
| **Subcommand** | `SUBCOMMAND` | Subcommand with its own options | `SUBCOMMAND("add", add_options, ...)` |
| **Array** | `OPTION_ARRAY_*` | Option with multiple values | `OPTION_ARRAY_STRING('i', "input", "Input files")` |
| **Map** | `OPTION_MAP_*` | Option with key-value pairs | `OPTION_MAP_STRING('m', "map", "Key-value pairs")` |

## Complete Example

Here's a more complete example including different types of options:

=== "Options Definition"
    ```c
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        VERSION_OPTION(FLAGS(FLAG_EXIT)),
        OPTION_FLAG('v', "verbose", "Enable verbose mode"),
        OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
        OPTION_INT('c', "count", "Number of iterations", RANGE(1, 100), DEFAULT(10)),
        OPTION_FLOAT('f', "factor", "Scale factor", DEFAULT(1.0)),
        POSITIONAL_STRING("input", "Input file")
    )
    ```

=== "Main Code"
    ```c
    #include "cargs.h"
    #include <stdio.h>
    #include <stdlib.h>

    // Options definition (see previous tab)

    int main(int argc, char **argv)
    {
        // Initialize cargs
        cargs_t cargs = cargs_init(options, "complete_example", "1.0.0");
        cargs.description = "Complete example of cargs";
        
        // Parse arguments
        int status = cargs_parse(&cargs, argc, argv);
        if (status != CARGS_SUCCESS) {
            return status;
        }
        
        // Access parsed values
        const char *input = cargs_get(cargs, "input").as_string;
        const char *output = cargs_get(cargs, "output").as_string;
        int count = cargs_get(cargs, "count").as_int;
        double factor = cargs_get(cargs, "factor").as_float;
        bool verbose = cargs_get(cargs, "verbose").as_bool;
        
        // Display configuration
        printf("Configuration:\n");
        printf("  Input: %s\n", input);
        printf("  Output: %s\n", output);
        printf("  Count: %d\n", count);
        printf("  Factor: %.2f\n", factor);
        printf("  Verbose: %s\n", verbose ? "yes" : "no");
        
        // Free resources
        cargs_free(&cargs);
        return 0;
    }
    ```

=== "Compilation"
    ```bash
    gcc -o complete_example complete_example.c -lcargs
    ```

## Automatically Generated Help

With the `HELP_OPTION` and `VERSION_OPTION` options, cargs automatically generates formatted help and version information when the user specifies `--help` or `--version`.

!!! example "Example of generated help"
    ```
    complete_example v1.0.0

    Complete example of cargs

    Usage: complete_example [OPTIONS] <input>

    Arguments:
      <input>                - Input file

    Options:
      -h, --help             - Display this help message (exit)
      -V, --version          - Display version information (exit)
      -v, --verbose          - Enable verbose mode
      -o, --output <STR>     - Output file (default: "output.txt")
      -c, --count <NUM>      - Number of iterations [1-100] (default: 10)
      -f, --factor <FLOAT>   - Scale factor (default: 1.00)
    ```

## Typical Development Flow

Developing an application with cargs typically follows this process:

```mermaid
graph LR
    A[Define options] --> B[Initialize cargs]
    B --> C[Parse arguments]
    C --> D{Success?}
    D -- No --> E[Handle error]
    D -- Yes --> F[Access values]
    F --> G[Application logic]
    G --> H[Free resources]
```

## Next Steps

Now that you've created a basic application with cargs, you can explore more advanced features:

- [Basic Options](basic-options.md) - Learn more about different option types
- [Subcommands](subcommands.md) - Create applications with subcommands like Git or Docker
- [Validation](validation.md) - Validate user input with built-in or custom validators
