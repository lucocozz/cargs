# Subcommands

cargs supports Git/Docker-style subcommands, allowing you to create complex and hierarchical command-line interfaces.

## Basic Concepts

Subcommands allow you to organize program functionality into distinct commands, each with its own options. Common examples:

- `git commit`, `git push`, `git pull`
- `docker run`, `docker build`, `docker pull`

## Defining Subcommands

To create an application with subcommands, you need to:

1. Define options for each subcommand
2. Define an action for each subcommand
3. Reference the subcommands in the main options

### 1. Define Subcommand Options

```c
// Options for the "add" subcommand
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", "Force addition"),
    POSITIONAL_STRING("file", "File to add")
)

// Options for the "remove" subcommand
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", "Recursive removal"),
    POSITIONAL_STRING("file", "File to remove")
)
```

### 2. Define Action Functions

```c
// Action for the "add" subcommand
int add_action(cargs_t *cargs, void *data)
{
    // Access subcommand options
    const char *file = cargs_get(*cargs, "add.file").as_string;
    bool force = cargs_get(*cargs, "add.force").as_bool;
    
    printf("Adding file: %s\n", file);
    if (force) printf("  with force\n");
    
    return 0;
}

// Action for the "remove" subcommand
int remove_action(cargs_t *cargs, void *data)
{
    // Access subcommand options
    const char *file = cargs_get(*cargs, "remove.file").as_string;
    bool recursive = cargs_get(*cargs, "remove.recursive").as_bool;
    
    printf("Removing file: %s\n", file);
    if (recursive) printf("  recursively\n");
    
    return 0;
}
```

### 3. Reference Subcommands in Main Options

```c
// Main options with subcommands
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global options
    OPTION_FLAG('v', "verbose", "Verbose mode"),
    
    // Subcommands
    SUBCOMMAND("add", add_options, 
               HELP("Add files"), 
               ACTION(add_action)),
    
    SUBCOMMAND("remove", remove_options, 
               HELP("Remove files"), 
               ACTION(remove_action))
)
```

## Processing Subcommands

The main code for handling subcommands looks like this:

```c
int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "subcommands_example", "1.0.0");
    cargs.description = "Example of using subcommands";

    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }

    // Check if a subcommand was specified
    if (cargs_has_command(cargs)) {
        // Execute the subcommand's action
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }

    cargs_free(&cargs);
    return status;
}
```

## Accessing Subcommand Options

There are several ways to access subcommand options:

### 1. Absolute Path (from anywhere)

```c
// Access with complete path
const char *file = cargs_get(cargs, "add.file").as_string;
bool force = cargs_get(cargs, "add.force").as_bool;
```

### 2. Relative Path (from a subcommand handler)

In a subcommand action function, you can access options by their name without a prefix:

```c
int add_action(cargs_t *cargs, void *data)
{
    // Simplified access (relative)
    const char *file = cargs_get(*cargs, "file").as_string;
    bool force = cargs_get(*cargs, "force").as_bool;
    
    // ...
}
```

### 3. Accessing Global Options

Options defined at the root level are accessible from subcommand handlers using a dot prefix `.`:

```c
int add_action(cargs_t *cargs, void *data)
{
    // Access to global options with a dot prefix
    bool verbose = cargs_get(*cargs, ".verbose").as_bool;
    
    // ...
}
```

## Nested Subcommands

cargs also supports nested subcommands (like `git remote add`):

```c
// Options for "remote add"
CARGS_OPTIONS(
    remote_add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_STRING("name", "Name of the remote repository"),
    POSITIONAL_STRING("url", "URL of the remote repository")
)

// Options for "remote"
CARGS_OPTIONS(
    remote_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    SUBCOMMAND("add", remote_add_options, 
               HELP("Add a remote repository"), 
               ACTION(remote_add_action))
)

// Main options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    SUBCOMMAND("remote", remote_options, 
               HELP("Manage remote repositories"))
)
```

Access to nested options is done using dot notation:

```c
const char *name = cargs_get(cargs, "remote.add.name").as_string;
const char *url = cargs_get(cargs, "remote.add.url").as_string;
```

## Navigating Between Subcommands

In an interactive CLI, you might need to check which subcommand was used:

```c
if (cargs_is_set(cargs, "add")) {
    // The "add" subcommand was used
} else if (cargs_is_set(cargs, "remove")) {
    // The "remove" subcommand was used
}

// For nested subcommands
if (cargs_is_set(cargs, "remote.add")) {
    // The "remote add" subcommand was used
}
```

## Complete Example

You can find a complete example in the `examples/subcommands.c` file of the cargs project.
