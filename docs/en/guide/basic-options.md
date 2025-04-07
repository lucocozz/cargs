# Basic Options

cargs supports different types of options to meet various needs. This page presents the fundamental option types.

## Main Option Types

cargs supports four fundamental types of command-line elements:

* **Options** - Elements with dash prefix
    * With values (`-o value`, `--option=value`)
    * Flags/Boolean (`-v`, `--verbose`)
* **Positionals** - Arguments without prefix
    * Required (must be provided)
    * Optional (can be omitted)
* **Groups** - Logical organization of options
    * Regular groups (visual grouping)
    * Exclusive groups (only one can be selected)
* **Subcommands** - Hierarchical commands
    * Simple commands
    * Nested commands

## Options with Values

### String Option

=== "Definition"
    ```c
    OPTION_STRING('o', "output", HELP("Output file"), 
                  DEFAULT("output.txt"),   // Default value
                  HINT("FILE"))            // Hint displayed in help
    ```

=== "Usage"
    ```bash
    $ ./my_program --output=file.txt
    $ ./my_program -o file.txt
    ```

=== "Accessing"
    ```c
    const char* output = cargs_get(cargs, "output").as_string;
    ```

### Integer Option

=== "Definition"
    ```c
    OPTION_INT('p', "port", HELP("Port number"), 
               RANGE(1, 65535),   // Range validation
               DEFAULT(8080))     // Default value
    ```

=== "Usage"
    ```bash
    $ ./my_program --port=8080
    $ ./my_program -p 8080
    ```

=== "Accessing"
    ```c
    int port = cargs_get(cargs, "port").as_int;
    ```

### Float Option

=== "Definition"
    ```c
    OPTION_FLOAT('s', "scale", HELP("Scale factor"), 
                 DEFAULT(1.0))    // Default value
    ```

=== "Usage"
    ```bash
    $ ./my_program --scale=2.5
    $ ./my_program -s 2.5
    ```

=== "Accessing"
    ```c
    float scale = cargs_get(cargs, "scale").as_float;
    ```

### Boolean Options
=== "Definition"
    ```c
    OPTION_BOOL('f', "force", HELP("Force operation"),
                 DEFAULT(false))  // Default value
    ```
=== "Usage"
    ```bash
    $ ./my_program --force=true
    $ ./my_program -f false
    ```
=== "Accessing"
    ```c
    bool force = cargs_get(cargs, "force").as_bool;
    ```

## Flag Options (no value)

=== "Definition"
    ```c
    OPTION_FLAG('v', "verbose", HELP("Enable verbose mode"))
    ```

=== "Usage"
    ```bash
    $ ./my_program --verbose
    $ ./my_program -v
    ```

=== "Accessing"
    ```c
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    ```

!!! info
    No value is required for boolean options; the presence of the option activates the flag.

## Standard Options

!!! note "Help Option"
    ```c
    HELP_OPTION(FLAGS(FLAG_EXIT))
    ```
    Creates a `-h, --help` option that automatically displays a help message and exits the program.

!!! tip "Version Option"
    ```c
    VERSION_OPTION(FLAGS(FLAG_EXIT))
    ```
    Creates a `-V, --version` option that displays the program version and exits.

## Positional Arguments

Positional arguments are ordered and not preceded by dashes.

=== "Required Positional"
    ```c
    POSITIONAL_STRING("input", HELP("Input file"))
    ```

=== "Optional Positional"
    ```c
    POSITIONAL_STRING("output", HELP("Output file"), 
                      FLAGS(FLAG_OPTIONAL),   // Optional argument
                      DEFAULT("output.txt"))  // Default value
    ```

=== "Usage"
    ```bash
    $ ./my_program input.txt [output.txt]
    ```

!!! warning "Important"
    Required positional arguments must always be defined before optional ones in your `CARGS_OPTIONS` definition. This ordering requirement is validated by cargs during initialization, and failure to follow this rule will result in an error.

For example, this is the correct order:

```c
CARGS_OPTIONS(
    options,
    // Required positional arguments first
    POSITIONAL_STRING("input", HELP("Input file")),                       // Required
    POSITIONAL_STRING("output", HELP("Output file")),                     // Required
    
    // Optional positional arguments after
    POSITIONAL_INT("buffer_size", HELP("Buffer size"), 
                   FLAGS(FLAG_OPTIONAL), DEFAULT(4096)),                  // Optional
    POSITIONAL_STRING("log_file", HELP("Log file"), FLAGS(FLAG_OPTIONAL)) // Optional
)
```

## Options with Short or Long Name Only

| Type | Definition | User Format | Access Code |
|------|------------|-------------|-------------|
| Short Name Only | `OPTION_INT('p', NULL, "Port number")` | `-p 8080` | `cargs_get(cargs, "p").as_int` |
| Long Name Only | `OPTION_FLAG('\0', "dry-run", "Run without changes")` | `--dry-run` | `cargs_get(cargs, "dry-run").as_bool` |

!!! tip "Accessing Options"
    When accessing option values with functions like `cargs_get()`, cargs uses a specific rule:
    
    - By default, cargs uses the **long name** as the identifier
    - If the long name is not set (NULL), it uses the **short name** as the identifier

## Option Groups

Groups allow you to visually organize options in the help:

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    GROUP_START("Connection", GROUP_DESC("Connection options")),
        OPTION_STRING('h', "host", HELP("Hostname"), DEFAULT("localhost")),
        OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080)),
    GROUP_END(),
    
    GROUP_START("Security", GROUP_DESC("Security options")),
        OPTION_STRING('u', "username", HELP("Username")),
        OPTION_STRING('P', "password", HELP("Password")),
        OPTION_FLAG('s', "secure", HELP("Use secure connection")),
    GROUP_END()
)
```

The generated help will display options organized by groups:

```
Connection options:
  -h, --host <STR>      - Hostname (default: "localhost")
  -p, --port <NUM>      - Port number (default: 8080)

Security options:
  -u, --username <STR>  - Username
  -P, --password <STR>  - Password
  -s, --secure          - Use secure connection
```

## Exclusive Groups

You can create groups of mutually exclusive options:

```c
GROUP_START("Compression", GROUP_DESC("Compression options"), 
            FLAGS(FLAG_EXCLUSIVE)),
    OPTION_FLAG('z', "gzip", HELP("Use gzip compression")),
    OPTION_FLAG('j', "bzip2", HELP("Use bzip2 compression")),
    OPTION_FLAG('Z', "lzma", HELP("Use lzma compression")),
GROUP_END()
```

With an exclusive group, the user can specify only one of the options in the group.

## Option Flags

Options can have various flags that modify their behavior:

| Flag | Description | Example |
|------|-------------|---------|
| `FLAG_REQUIRED` | Option must be specified | `FLAGS(FLAG_REQUIRED)` |
| `FLAG_HIDDEN` | Option is hidden in help | `FLAGS(FLAG_HIDDEN)` |
| `FLAG_EXIT` | Program terminates after processing | `FLAGS(FLAG_EXIT)` |
| `FLAG_ADVANCED` | Option is marked as advanced | `FLAGS(FLAG_ADVANCED)` |
| `FLAG_DEPRECATED` | Option is marked as deprecated | `FLAGS(FLAG_DEPRECATED)` |
| `FLAG_EXPERIMENTAL` | Option is marked as experimental | `FLAGS(FLAG_EXPERIMENTAL)` |

Multiple flags can be combined using the bitwise OR operator:

```c
OPTION_STRING('t', "temp-dir", HELP("Temporary directory"),
              FLAGS(FLAG_ADVANCED | FLAG_EXPERIMENTAL))
```

## Accessing Option Values

After parsing the arguments, you can access option values:

```c
// Get a string value
const char *output = cargs_get(cargs, "output").as_string;

// Get an integer value
int port = cargs_get(cargs, "port").as_int;

// Get a float value
float scale = cargs_get(cargs, "scale").as_float;

// Check if a flag is enabled
bool verbose = cargs_get(cargs, "verbose").as_bool;

// Check if an option was explicitly set
if (cargs_is_set(cargs, "output")) {
    // The output option was specified by the user
}
```

## Options with Dependencies

You can define dependencies between options:

```c
// The username option requires password
OPTION_STRING('u', "username", HELP("Username"), 
              REQUIRES("password"))

// The password option requires username
OPTION_STRING('p', "password", HELP("Password"), 
              REQUIRES("username"))
```

## Incompatible Options

You can define incompatibilities between options:

```c
// The verbose option is incompatible with quiet
OPTION_FLAG('v', "verbose", HELP("Verbose mode"), 
            CONFLICTS("quiet"))

// The quiet option is incompatible with verbose
OPTION_FLAG('q', "quiet", HELP("Quiet mode"), 
            CONFLICTS("verbose"))
```

## Complete Example

```c
#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Standard options
    OPTION_FLAG('v', "verbose", HELP("Enable verbose mode")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535), DEFAULT(8080)),
    OPTION_FLOAT('s', "scale", HELP("Scale factor"), DEFAULT(1.0)),
    
    // Exclusive options
    GROUP_START("Mode", GROUP_DESC("Mode options"), FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('d', "debug", HELP("Debug mode")),
        OPTION_FLAG('r', "release", HELP("Release mode")),
    GROUP_END(),
    
    // Options with dependencies
    OPTION_STRING('u', "username", HELP("Username"), REQUIRES("password")),
    OPTION_STRING('P', "password", HELP("Password"), REQUIRES("username")),
    
    // Positional arguments
    POSITIONAL_STRING("input", HELP("Input file"))
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Access values
    const char *input = cargs_get(cargs, "input").as_string;
    const char *output = cargs_get(cargs, "output").as_string;
    int port = cargs_get(cargs, "port").as_int;
    float scale = cargs_get(cargs, "scale").as_float;
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    
    // Check exclusive options
    bool debug = cargs_get(cargs, "debug").as_bool;
    bool release = cargs_get(cargs, "release").as_bool;
    
    // Authentication if specified
    if (cargs_is_set(cargs, "username")) {
        const char *username = cargs_get(cargs, "username").as_string;
        const char *password = cargs_get(cargs, "password").as_string;
        
        printf("Authenticating with %s\n", username);
    }
    
    // Free resources
    cargs_free(&cargs);
    return 0;
}
```

When run with `--help`, this example will generate a well-formatted help display with all the options properly organized.
