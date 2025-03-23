# Basic Options

cargs supports different types of options to meet various needs. This page presents the fundamental option types.

## Main Option Types

There are four main types of inputs:

1. **Options** - Options with dash prefix (`-o` or `--option`)
2. **Positionals** - Arguments without prefix placed in a specific order
3. **Groups** - Logical groupings of options
4. **Subcommands** - Hierarchical commands with their own options

## Options with Values

### String Option

```c
OPTION_STRING('o', "output", "Output file", 
              DEFAULT("output.txt"),   // Default value
              HINT("FILE"))            // Hint displayed in help
```

- User format: `--output=file.txt` or `-o file.txt`

### Integer Option

```c
OPTION_INT('p', "port", "Port number", 
           RANGE(1, 65535),   // Range validation
           DEFAULT(8080))     // Default value
```

- User format: `--port=8080` or `-p 8080`

### Float Option

```c
OPTION_FLOAT('s', "scale", "Scale factor", 
             DEFAULT(1.0))    // Default value
```

- User format: `--scale=2.5` or `-s 2.5`

## Boolean Options (flags)

```c
OPTION_FLAG('v', "verbose", "Enable verbose mode")
```

- User format: `--verbose` or `-v`
- No value required, the presence of the option activates the flag

## Standard Options

### Help Option

```c
HELP_OPTION(FLAGS(FLAG_EXIT))
```

Creates a `-h, --help` option that automatically displays a help message and exits the program.

### Version Option

```c
VERSION_OPTION(FLAGS(FLAG_EXIT))
```

Creates a `-V, --version` option that displays the program version and exits.

## Positional Arguments

Positional arguments are ordered and not preceded by dashes.

```c
POSITIONAL_STRING("input", "Input file")
```

- User format: `my_program file.txt`

You can define optional positional arguments:

```c
POSITIONAL_STRING("output", "Output file", 
                  FLAGS(FLAG_OPTIONAL),   // Optional argument
                  DEFAULT("output.txt"))  // Default value
```

Required positional arguments must always be defined before optional ones.

## Options with Short or Long Name Only

### Option with Short Name Only

```c
OPTION_INT('p', NULL, "Port number", 
           DEFAULT(8080))
```

- User format: `-p 8080`

### Option with Long Name Only

```c
OPTION_FLAG('\0', "dry-run", "Run without applying changes")
```

- User format: `--dry-run`

## Option Groups

Groups allow you to visually organize options in the help:

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    GROUP_START("Connection", GROUP_DESC("Connection options")),
        OPTION_STRING('h', "host", "Hostname", DEFAULT("localhost")),
        OPTION_INT('p', "port", "Port number", DEFAULT(8080)),
    GROUP_END(),
    
    GROUP_START("Security", GROUP_DESC("Security options")),
        OPTION_STRING('u', "username", "Username"),
        OPTION_STRING('P', "password", "Password"),
        OPTION_FLAG('s', "secure", "Use secure connection"),
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
    OPTION_FLAG('z', "gzip", "Use gzip compression"),
    OPTION_FLAG('j', "bzip2", "Use bzip2 compression"),
    OPTION_FLAG('Z', "lzma", "Use lzma compression"),
GROUP_END()
```

With an exclusive group, the user can specify only one of the options in the group.

## Option Flags

Options can have various flags that modify their behavior:

```c
OPTION_STRING('o', "output", "Output file",
              FLAGS(FLAG_REQUIRED))  // Required option
```

Common flags:

- `FLAG_REQUIRED` - The option must be specified
- `FLAG_HIDDEN` - The option is hidden in help
- `FLAG_EXIT` - The program terminates after processing the option
- `FLAG_ADVANCED` - The option is marked as advanced

## Accessing Option Values

After parsing the arguments, you can access option values:

```c
// Get a string value
const char *output = cargs_get(cargs, "output").as_string;

// Get an integer value
int port = cargs_get(cargs, "port").as_int;

// Get a float value
double scale = cargs_get(cargs, "scale").as_float;

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
OPTION_STRING('u', "username", "Username", 
              REQUIRES("password"))

// The password option requires username
OPTION_STRING('p', "password", "Password", 
              REQUIRES("username"))
```

## Incompatible Options

You can define incompatibilities between options:

```c
// The verbose option is incompatible with quiet
OPTION_FLAG('v', "verbose", "Verbose mode", 
            CONFLICTS("quiet"))

// The quiet option is incompatible with verbose
OPTION_FLAG('q', "quiet", "Quiet mode", 
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
    OPTION_FLAG('v', "verbose", "Enable verbose mode"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
    OPTION_INT('p', "port", "Port number", RANGE(1, 65535), DEFAULT(8080)),
    OPTION_FLOAT('s', "scale", "Scale factor", DEFAULT(1.0)),
    
    // Exclusive options
    GROUP_START("Mode", GROUP_DESC("Mode options"), FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('d', "debug", "Debug mode"),
        OPTION_FLAG('r', "release", "Release mode"),
    GROUP_END(),
    
    // Options with dependencies
    OPTION_STRING('u', "username", "Username", REQUIRES("password")),
    OPTION_STRING('P', "password", "Password", REQUIRES("username")),
    
    // Positional arguments
    POSITIONAL_STRING("input", "Input file")
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
    double scale = cargs_get(cargs, "scale").as_float;
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
