# Macros

This page details the macros available in cargs to define options and customize their behavior.

## Main Macro

### CARGS_OPTIONS

The main macro for defining a set of options:

```c
CARGS_OPTIONS(name, ...)
```

| Parameter | Description |
|-----------|-------------|
| `name` | Name of the variable that will hold the options array |
| `...` | List of options defined with the macros below |

Example:
```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose mode")
)
```

## Option Definition Macros

### Options with Values

#### OPTION_STRING

Defines a string option:

```c
OPTION_STRING(short_name, long_name, help, ...)
```

| Parameter | Description |
|-----------|-------------|
| `short_name` | Character for short name (e.g., 'o' for -o), or '\0' if none |
| `long_name` | String for long name (e.g., "output" for --output), or NULL if none |
| `help` | Help text describing the option |
| `...` | Optional modifiers (DEFAULT, HINT, FLAGS, etc.) |

Example:
```c
OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt"))
```

#### OPTION_INT

Defines an integer option:

```c
OPTION_INT(short_name, long_name, help, ...)
```

Parameters identical to `OPTION_STRING` but for an integer value.

Example:
```c
OPTION_INT('p', "port", "Port number", RANGE(1, 65535), DEFAULT(8080))
```

#### OPTION_FLOAT

Defines a floating-point option:

```c
OPTION_FLOAT(short_name, long_name, help, ...)
```

Parameters identical to `OPTION_STRING` but for a floating-point value.

Example:
```c
OPTION_FLOAT('s', "scale", "Scale factor", DEFAULT(1.0))
```

### Boolean Option

#### OPTION_FLAG

Defines a boolean option (flag) that doesn't accept a value:

```c
OPTION_FLAG(short_name, long_name, help, ...)
```

Parameters identical to options with values, but no value expected.

Example:
```c
OPTION_FLAG('v', "verbose", "Enable verbose mode")
```

### Collection Options (Arrays)

#### OPTION_ARRAY_STRING

Defines an option accepting an array of strings:

```c
OPTION_ARRAY_STRING(short_name, long_name, help, ...)
```

Example:
```c
OPTION_ARRAY_STRING('n', "names", "List of names", FLAGS(FLAG_SORTED))
```

#### OPTION_ARRAY_INT

Defines an option accepting an array of integers:

```c
OPTION_ARRAY_INT(short_name, long_name, help, ...)
```

Example:
```c
OPTION_ARRAY_INT('i', "ids", "List of IDs", FLAGS(FLAG_UNIQUE | FLAG_SORTED))
```

#### OPTION_ARRAY_FLOAT

Defines an option accepting an array of floating-point values:

```c
OPTION_ARRAY_FLOAT(short_name, long_name, help, ...)
```

Example:
```c
OPTION_ARRAY_FLOAT('f', "factors", "List of factors")
```

### Collection Options (Maps)

#### OPTION_MAP_STRING

Defines an option accepting a map of strings:

```c
OPTION_MAP_STRING(short_name, long_name, help, ...)
```

Example:
```c
OPTION_MAP_STRING('e', "env", "Environment variables", FLAGS(FLAG_SORTED_KEY))
```

#### OPTION_MAP_INT

Defines an option accepting a map of integers:

```c
OPTION_MAP_INT(short_name, long_name, help, ...)
```

Example:
```c
OPTION_MAP_INT('p', "ports", "Service ports", FLAGS(FLAG_SORTED_VALUE))
```

#### OPTION_MAP_FLOAT

Defines an option accepting a map of floating-point values:

```c
OPTION_MAP_FLOAT(short_name, long_name, help, ...)
```

Example:
```c
OPTION_MAP_FLOAT('s', "scales", "Scale factors by dimension")
```

#### OPTION_MAP_BOOL

Defines an option accepting a map of booleans:

```c
OPTION_MAP_BOOL(short_name, long_name, help, ...)
```

Example:
```c
OPTION_MAP_BOOL('f', "features", "Feature configuration")
```

### Positional Arguments

#### POSITIONAL_STRING

Defines a string positional argument:

```c
POSITIONAL_STRING(name, help, ...)
```

| Parameter | Description |
|-----------|-------------|
| `name` | Argument name |
| `help` | Help text describing the argument |
| `...` | Optional modifiers (FLAGS, DEFAULT, etc.) |

Example:
```c
POSITIONAL_STRING("input", "Input file")
```

#### POSITIONAL_INT

Defines an integer positional argument:

```c
POSITIONAL_INT(name, help, ...)
```

Example:
```c
POSITIONAL_INT("count", "Number of iterations", DEFAULT(1))
```

#### POSITIONAL_FLOAT

Defines a floating-point positional argument:

```c
POSITIONAL_FLOAT(name, help, ...)
```

Example:
```c
POSITIONAL_FLOAT("threshold", "Detection threshold", DEFAULT(0.5))
```

#### POSITIONAL_BOOL

Defines a boolean positional argument:

```c
POSITIONAL_BOOL(name, help, ...)
```

Example:
```c
POSITIONAL_BOOL("enabled", "Enable feature")
```

### Standard Options

#### HELP_OPTION

Defines a help option (`-h, --help`):

```c
HELP_OPTION(...)
```

Example:
```c
HELP_OPTION(FLAGS(FLAG_EXIT))
```

#### VERSION_OPTION

Defines a version option (`-V, --version`):

```c
VERSION_OPTION(...)
```

Example:
```c
VERSION_OPTION(FLAGS(FLAG_EXIT))
```

### Subcommands and Groups

#### SUBCOMMAND

Defines a subcommand with its own options:

```c
SUBCOMMAND(name, sub_options, ...)
```

| Parameter | Description |
|-----------|-------------|
| `name` | Name of the subcommand |
| `sub_options` | Options array for the subcommand |
| `...` | Optional modifiers (HELP, ACTION, etc.) |

Example:
```c
SUBCOMMAND("add", add_options, HELP("Add a file"), ACTION(add_action))
```

#### GROUP_START

Starts an options group:

```c
GROUP_START(name, ...)
```

| Parameter | Description |
|-----------|-------------|
| `name` | Group name |
| `...` | Optional modifiers (GROUP_DESC, FLAGS, etc.) |

Example:
```c
GROUP_START("Connection", GROUP_DESC("Connection options"))
```

#### GROUP_END

Ends an options group:

```c
GROUP_END()
```

## Customization Macros

### DEFAULT

Sets a default value for an option:

```c
DEFAULT(val)
```

Example:
```c
OPTION_INT('p', "port", "Port number", DEFAULT(8080))
```

### HINT

Sets a hint displayed in the help for the value format:

```c
HINT(text)
```

Example:
```c
OPTION_STRING('o', "output", "Output file", HINT("FILE"))
```

### FLAGS

Sets flags to modify an option's behavior:

```c
FLAGS(flags)
```

Example:
```c
OPTION_STRING('o', "output", "Output file", FLAGS(FLAG_REQUIRED))
```

### REQUIRES

Defines dependencies between options:

```c
REQUIRES(...)
```

Example:
```c
OPTION_STRING('u', "username", "Username", REQUIRES("password"))
```

### CONFLICTS

Defines incompatibilities between options:

```c
CONFLICTS(...)
```

Example:
```c
OPTION_FLAG('v', "verbose", "Verbose mode", CONFLICTS("quiet"))
```

### RANGE

Defines a valid range of values for a numeric option:

```c
RANGE(min, max)
```

Example:
```c
OPTION_INT('p', "port", "Port number", RANGE(1, 65535))
```

### CHOICES_STRING, CHOICES_INT, CHOICES_FLOAT

Defines valid choices for an option:

```c
CHOICES_STRING(...)
CHOICES_INT(...)
CHOICES_FLOAT(...)
```

Example:
```c
OPTION_STRING('l', "level", "Logging level", 
             CHOICES_STRING("debug", "info", "warning", "error"))
```

### REGEX

Applies regular expression validation:

```c
REGEX(pattern)
```

Example:
```c
OPTION_STRING('e', "email", "Email address", REGEX(CARGS_RE_EMAIL))
```

### MAKE_REGEX

Creates a regular expression pattern with explanation:

```c
MAKE_REGEX(pattern, hint)
```

Example:
```c
REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000"))
```

### ENV_VAR

Specifies an environment variable associated with the option:

```c
ENV_VAR(name)
```

Example:
```c
OPTION_STRING('H', "host", "Hostname", ENV_VAR("HOST"))
```

### VALIDATOR

Associates a custom validator with an option:

```c
VALIDATOR(fn, data)
```

Example:
```c
VALIDATOR(even_validator, NULL)
```

### PRE_VALIDATOR

Associates a custom pre-validator with an option:

```c
PRE_VALIDATOR(fn, data)
```

Example:
```c
PRE_VALIDATOR(length_validator, &min_length)
```

### HANDLER

Specifies a custom handler for an option:

```c
HANDLER(fn)
```

Example:
```c
HANDLER(ipv4_handler)
```

### ACTION

Associates an action with a subcommand:

```c
ACTION(fn)
```

Example:
```c
ACTION(add_command)
```
