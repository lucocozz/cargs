# Macros Reference

This page provides a comprehensive reference for all the macros available in cargs to define options and customize their behavior.

!!! abstract "Overview"
    cargs uses a macro-based approach to define command-line options, making it easy to specify option properties in a concise, declarative style. The macros are organized into several categories:
    
    - **Definition Macros** - Define options of various types
    - **Option Properties** - Customize option behavior
    - **Groups and Subcommands** - Organize options and create command hierarchies
    - **Validator Macros** - Add validation constraints
    - **Flag Combinations** - Configure option flags

## Main Definition Macro

### CARGS_OPTIONS

This is the main macro that defines a set of command-line options:

```c
CARGS_OPTIONS(name, ...)
```

**Parameters:**
- `name`: Name of the variable that will hold the options array
- `...`: List of options defined with the macros below

**Example:**
```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output"))
)
```

## Option Definition Macros

### Standard Options

#### OPTION_STRING

Defines a string option:

```c
OPTION_STRING(short_name, long_name, help, ...)
```

**Parameters:**
- `short_name`: Character for short name (e.g., 'o' for -o), or '\0' if none
- `long_name`: String for long name (e.g., "output" for --output), or NULL if none
- `help`: Help text describing the option (use HELP() macro)
- `...`: Optional modifiers (DEFAULT, HINT, FLAGS, etc.)

**Example:**
```c
OPTION_STRING('o', "output", HELP("Output file"), 
             DEFAULT("output.txt"), 
             HINT("FILE"))
```

#### OPTION_INT

Defines an integer option:

```c
OPTION_INT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_INT('p', "port", HELP("Port number"), 
           DEFAULT(8080), 
           RANGE(1, 65535))
```

#### OPTION_FLOAT

Defines a floating-point option:

```c
OPTION_FLOAT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_FLOAT('f', "factor", HELP("Scaling factor"), 
             DEFAULT(1.0))
```

#### OPTION_BOOL
Defines a boolean option:

```c
OPTION_BOOL(short_name, long_name, help, ...)
```
**Parameters:** Same as `OPTION_STRING`
**Example:**
```c
OPTION_BOOL('d', "debug", HELP("Enable debug mode"), 
            DEFAULT(false))
```

#### OPTION_FLAG

Defines a boolean option (flag) that doesn't take a value:

```c
OPTION_FLAG(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_FLAG('v', "verbose", HELP("Enable verbose output"))
```

### Array Options

#### OPTION_ARRAY_STRING

Defines an option accepting an array of strings:

```c
OPTION_ARRAY_STRING(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_ARRAY_STRING('t', "tags", HELP("Tags for the resource"), 
                   FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

#### OPTION_ARRAY_INT

Defines an option accepting an array of integers:

```c
OPTION_ARRAY_INT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_ARRAY_INT('p', "ports", HELP("Port numbers"), 
                FLAGS(FLAG_UNIQUE))
```

#### OPTION_ARRAY_FLOAT

Defines an option accepting an array of floating-point values:

```c
OPTION_ARRAY_FLOAT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_ARRAY_FLOAT('f', "factors", HELP("Scaling factors"))
```

### Map Options

#### OPTION_MAP_STRING

Defines an option accepting a map of strings:

```c
OPTION_MAP_STRING(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_MAP_STRING('e', "env", HELP("Environment variables"), 
                 FLAGS(FLAG_SORTED_KEY))
```

#### OPTION_MAP_INT

Defines an option accepting a map of integers:

```c
OPTION_MAP_INT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_MAP_INT('p', "ports", HELP("Service ports"), 
              FLAGS(FLAG_SORTED_KEY))
```

#### OPTION_MAP_FLOAT

Defines an option accepting a map of floating-point values:

```c
OPTION_MAP_FLOAT(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_MAP_FLOAT('s', "scales", HELP("Scale factors by dimension"))
```

#### OPTION_MAP_BOOL

Defines an option accepting a map of booleans:

```c
OPTION_MAP_BOOL(short_name, long_name, help, ...)
```

**Parameters:** Same as `OPTION_STRING`

**Example:**
```c
OPTION_MAP_BOOL('f', "features", HELP("Feature toggles"))
```

### Positional Arguments

#### POSITIONAL_STRING

Defines a string positional argument:

```c
POSITIONAL_STRING(name, help, ...)
```

**Parameters:**
- `name`: Argument name (for references and help display)
- `help`: Help text describing the argument
- `...`: Optional modifiers (FLAGS, DEFAULT, etc.)

**Example:**
```c
POSITIONAL_STRING("input", HELP("Input file"))
```

#### POSITIONAL_INT

Defines an integer positional argument:

```c
POSITIONAL_INT(name, help, ...)
```

**Parameters:** Same as `POSITIONAL_STRING`

**Example:**
```c
POSITIONAL_INT("count", HELP("Number of iterations"), 
               DEFAULT(1))
```

#### POSITIONAL_FLOAT

Defines a floating-point positional argument:

```c
POSITIONAL_FLOAT(name, help, ...)
```

**Parameters:** Same as `POSITIONAL_STRING`

**Example:**
```c
POSITIONAL_FLOAT("threshold", HELP("Detection threshold"), 
                 DEFAULT(0.5))
```

#### POSITIONAL_BOOL

Defines a boolean positional argument:

```c
POSITIONAL_BOOL(name, help, ...)
```

**Parameters:** Same as `POSITIONAL_STRING`

**Example:**
```c
POSITIONAL_BOOL("enabled", HELP("Enable feature"))
```

### Common Options

#### HELP_OPTION

Defines a help option (-h, --help):

```c
HELP_OPTION(...)
```

**Parameters:**
- `...`: Optional modifiers (typically FLAGS(FLAG_EXIT))

**Example:**
```c
HELP_OPTION(FLAGS(FLAG_EXIT))
```

#### VERSION_OPTION

Defines a version option (-V, --version):

```c
VERSION_OPTION(...)
```

**Parameters:**
- `...`: Optional modifiers (typically FLAGS(FLAG_EXIT))

**Example:**
```c
VERSION_OPTION(FLAGS(FLAG_EXIT))
```

### Base Options

#### OPTION_BASE

Defines a custom option with more control:

```c
OPTION_BASE(short_name, long_name, help, value_type, ...)
```

**Parameters:**
- `short_name`: Character for short name (e.g., 'o' for -o), or '\0' if none
- `long_name`: String for long name (e.g., "output" for --output), or NULL if none
- `help`: Help text describing the option
- `value_type`: Type of value (VALUE_TYPE_STRING, VALUE_TYPE_INT, etc.)
- `...`: Optional modifiers (HANDLER, FREE_HANDLER, etc.)

**Example:**
```c
OPTION_BASE('i', "ip", HELP("IP address"), VALUE_TYPE_CUSTOM,
            HANDLER(ip_handler),
            FREE_HANDLER(ip_free_handler))
```

#### POSITIONAL_BASE

Defines a custom positional argument:

```c
POSITIONAL_BASE(name, help, value_type, ...)
```

**Parameters:**
- `name`: Argument name
- `help`: Help text describing the argument
- `value_type`: Type of value
- `...`: Optional modifiers

**Example:**
```c
POSITIONAL_BASE("coordinate", HELP("Point coordinates"), VALUE_TYPE_CUSTOM,
                HANDLER(coordinate_handler),
                FREE_HANDLER(coordinate_free_handler))
```

## Group and Subcommand Macros

### GROUP_START

Starts an options group:

```c
GROUP_START(name, ...)
```

**Parameters:**
- `name`: Group name
- `...`: Optional modifiers (GROUP_DESC, FLAGS, etc.)

**Example:**
```c
GROUP_START("Connection", GROUP_DESC("Connection options"))
```

### GROUP_END

Ends an options group:

```c
GROUP_END()
```

**Example:**
```c
GROUP_END()
```

### SUBCOMMAND

Defines a subcommand with its own options:

```c
SUBCOMMAND(name, sub_options, ...)
```

**Parameters:**
- `name`: Name of the subcommand
- `sub_options`: Options array for the subcommand
- `...`: Optional modifiers (HELP, ACTION, etc.)

**Example:**
```c
SUBCOMMAND("add", add_options, 
           HELP("Add files"), 
           ACTION(add_command))
```

## Property Macros

### DEFAULT

Sets a default value for an option:

```c
DEFAULT(value)
```

**Example:**
```c
OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080))
```

### HINT

Sets a hint displayed in the help for the value format:

```c
HINT(text)
```

**Example:**
```c
OPTION_STRING('o', "output", HELP("Output file"), HINT("FILE"))
```

### HELP

Sets the help text for a subcommand:

```c
HELP(text)
```

**Example:**
```c
SUBCOMMAND("add", add_options, HELP("Add files to the repository"))
```

### GROUP_DESC

Sets the description for an option group:

```c
GROUP_DESC(text)
```

**Example:**
```c
GROUP_START("Advanced", GROUP_DESC("Advanced configuration options"))
```

### FLAGS

Sets flags to modify an option's behavior:

```c
FLAGS(flags)
```

**Example:**
```c
OPTION_STRING('o', "output", HELP("Output file"), FLAGS(FLAG_REQUIRED))
```

### REQUIRES

Defines dependencies between options:

```c
REQUIRES(...)
```

**Example:**
```c
OPTION_STRING('u', "username", HELP("Username"), REQUIRES("password"))
```

### CONFLICTS

Defines incompatibilities between options:

```c
CONFLICTS(...)
```

**Example:**
```c
OPTION_FLAG('v', "verbose", HELP("Verbose mode"), CONFLICTS("quiet"))
```

### ENV_VAR

Specifies an environment variable associated with the option:

```c
ENV_VAR(name)
```

**Example:**
```c
OPTION_STRING('H', "host", HELP("Hostname"), ENV_VAR("HOST"))
```

### ACTION

Associates an action function with a subcommand:

```c
ACTION(function)
```

**Example:**
```c
SUBCOMMAND("add", add_options, ACTION(add_command))
```

### HANDLER

Specifies a custom handler for an option:

```c
HANDLER(function)
```

**Example:**
```c
HANDLER(ip_handler)
```

### FREE_HANDLER

Specifies a custom free handler for an option:

```c
FREE_HANDLER(function)
```

**Example:**
```c
FREE_HANDLER(ip_free_handler)
```

## Validator Macros

### RANGE

Defines a valid range of values for a numeric option:

```c
RANGE(min, max)
```

**Example:**
```c
OPTION_INT('p', "port", HELP("Port number"), RANGE(1, 65535))
```

### CHOICES_STRING, CHOICES_INT, CHOICES_FLOAT

Defines valid choices for an option:

```c
CHOICES_STRING(...)
CHOICES_INT(...)
CHOICES_FLOAT(...)
```

**Example:**
```c
OPTION_STRING('l', "level", HELP("Logging level"), 
             CHOICES_STRING("debug", "info", "warning", "error"))
```

### REGEX

Applies regular expression validation:

```c
REGEX(pattern)
```

**Example:**
```c
OPTION_STRING('e', "email", HELP("Email address"), REGEX(CARGS_RE_EMAIL))
```

### MAKE_REGEX

Creates a regular expression pattern with explanation:

```c
MAKE_REGEX(pattern, hint)
```

**Example:**
```c
REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000"))
```

### VALIDATOR

Associates a custom validator with an option:

```c
VALIDATOR(function, data)
```

**Example:**
```c
VALIDATOR(even_validator, NULL)
```

### PRE_VALIDATOR

Associates a custom pre-validator with an option:

```c
PRE_VALIDATOR(function, data)
```

**Example:**
```c
PRE_VALIDATOR(length_validator, &min_length)
```

## Flag Constants

These constants can be combined with the `FLAGS()` macro:

### Option Flags

| Flag | Description |
|------|-------------|
| `FLAG_REQUIRED` | Option must be specified |
| `FLAG_HIDDEN` | Option is hidden from help |
| `FLAG_ADVANCED` | Option is marked as advanced |
| `FLAG_DEPRECATED` | Option is marked as deprecated |
| `FLAG_EXPERIMENTAL` | Option is marked as experimental |
| `FLAG_EXIT` | Program terminates after handling this option |
| `FLAG_ENV_OVERRIDE` | Environment variable can override command-line value |
| `FLAG_AUTO_ENV` | Automatically generate environment variable name |
| `FLAG_NO_ENV_PREFIX` | Don't use environment variable prefix |

### Array and Map Flags

| Flag | Description |
|------|-------------|
| `FLAG_SORTED` | Sort array values |
| `FLAG_UNIQUE` | Remove duplicate array values |
| `FLAG_SORTED_KEY` | Sort map entries by key |
| `FLAG_SORTED_VALUE` | Sort map entries by value |
| `FLAG_UNIQUE_VALUE` | Ensure map has no duplicate values |

### Group Flags

| Flag | Description |
|------|-------------|
| `FLAG_EXCLUSIVE` | Only one option in the group can be selected |

### Helper Flag

| Flag | Description |
|------|-------------|
| `FLAG_OPTIONAL` | For positional arguments only, marks as optional |

## Complete Example

Here's a complete example showing various macros in use:

```c
// Define options for the "add" subcommand
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Force add operation"), 
               CONFLICTS("dry-run")),
    POSITIONAL_STRING("file", HELP("File to add"))
)

// Define options for the "remove" subcommand
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", HELP("Recursively remove directories")),
    POSITIONAL_STRING("file", HELP("File to remove"))
)

// Define main options with subcommands and option groups
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global options
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Log file"), 
                 DEFAULT("output.log"), 
                 HINT("FILE")),
    
    // Options in a group
    GROUP_START("Display", GROUP_DESC("Display options")),
        OPTION_FLAG('q', "quiet", HELP("Suppress output"), 
                   CONFLICTS("verbose")),
        OPTION_FLAG('c', "color", HELP("Colorize output")),
    GROUP_END(),
    
    // Options in an exclusive group (only one can be selected)
    GROUP_START("Format", GROUP_DESC("Output format"), 
                FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('j', "json", HELP("JSON output")),
        OPTION_FLAG('x', "xml", HELP("XML output")),
        OPTION_FLAG('y', "yaml", HELP("YAML output")),
    GROUP_END(),
    
    // Subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files to the index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Remove files from the index"), 
               ACTION(remove_command))
)
```

## Related Documentation

- [Functions Reference](functions.md) - Comprehensive API function reference
- [Types Reference](types.md) - Detailed information about data types
- [Regex Patterns](regex_patterns.md) - Predefined regular expression patterns
