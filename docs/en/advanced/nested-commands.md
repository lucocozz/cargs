# Nested Commands

Nested commands extend the basic subcommand concept to create rich, hierarchical command-line interfaces with multiple levels of command nesting and advanced command handling.

!!! abstract "Overview"
    This guide covers advanced command-line interface features in cargs:
    
    - **Nested Command Hierarchies** - Multiple levels of commands
    - **Command Abbreviations** - Support for shortened command names
    - **Flexible Argument Placement** - Positional arguments before and after commands
    - **Advanced Path Access** - Different ways to access options in complex hierarchies
    
    For basic subcommand usage, see the [Subcommands guide](../guide/subcommands.md).

## Nested Command Hierarchies

While basic subcommands provide a single level of command hierarchy, nested commands allow multiple levels:

```
my_app service create    # Two levels: "service" and "create"
my_app config set key value    # Two levels with positional args
my_app remote add origin https://example.com    # Multi-level hierarchy
```

This advanced feature is particularly useful for complex applications with many logically grouped commands, similar to tools like Git, Docker, or Kubernetes.

### Defining Nested Commands

To implement nested commands, you create a hierarchy of command definitions:

```c
// Define options for "service create" command
CARGS_OPTIONS(
    service_create_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('n', "name", HELP("Service name"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", HELP("Container image"), FLAGS(FLAG_REQUIRED))
)

// Define options for "service" command group
CARGS_OPTIONS(
    service_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("create", service_create_options, 
               HELP("Create a new service"), 
               ACTION(service_create_action))
    // Other service subcommands...
)

// Define main options with top-level subcommands
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global options at root level
    OPTION_FLAG('d', "debug", HELP("Enable debug mode")),
    
    SUBCOMMAND("service", service_options, 
               HELP("Service management commands"))
)
```

## Command Abbreviations

cargs supports command name abbreviations, allowing users to type shortened versions of command names as long as they are unambiguous:

=== "Definition"
    ```c
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("install", install_options, 
                   HELP("Install a package")),
        SUBCOMMAND("init", init_options, 
                   HELP("Initialize configuration"))
    )
    ```

=== "Usage Examples"
    ```bash
    # All of these are equivalent:
    my_program install package.tgz
    my_program ins package.tgz
    my_program i package.tgz    # ERROR: Ambiguous (could be "install" or "init")
    
    # For "init", these are equivalent:
    my_program init config.json
    my_program ini config.json
    ```

This behavior is similar to the command abbreviation found in tools like `ip` where `ip route add` can be abbreviated as `ip r a`.

!!! note "Ambiguous Abbreviations"
    If an abbreviation matches multiple commands, cargs will report an error.
    
    For example, with commands `status` and `start`, typing `sta` would be ambiguous.

### Implementing Command Abbreviation

Command abbreviation is built into cargs and doesn't require special configuration. cargs matches the command by finding the longest unique prefix:

1. User enters `ip r a 192.168.1.0/24`
2. cargs looks for commands starting with `r` and finds `route`
3. cargs matches this to the `route` command
4. Inside the `route` command, it looks for commands starting with `a` and finds `add`
5. The command is processed as `ip route add 192.168.1.0/24`

## Flexible Positional Argument Placement

In complex command structures, cargs supports flexible placement of positional arguments:

```
program pos1 pos2 command subcmd pos3 pos4
```

This allows for intuitive command structures where some positional arguments make sense before command selection, while others belong after.

=== "Definition"
    ```c
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        // Global positional arguments
        POSITIONAL_STRING("source", HELP("Source directory")),
        
        // Commands with their own positional arguments
        SUBCOMMAND("copy", copy_options,
                   HELP("Copy files"))
    )
    
    CARGS_OPTIONS(
        copy_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        POSITIONAL_STRING("destination", HELP("Destination directory"))
    )
    ```

=== "Usage Example"
    ```bash
    # Valid usages:
    my_program /source/dir copy /dest/dir
    my_program copy /dest/dir     # Uses default source
    ```

### Accessing Global and Command-specific Positionals

When using positional arguments at different levels, access them with appropriate paths:

```c
int copy_command(cargs_t *cargs, void *data)
{
    // Global positional defined before the command
    const char* source = cargs_get(*cargs, ".source").as_string;
    
    // Command-specific positional
    const char* destination = cargs_get(*cargs, "destination").as_string;
    
    printf("Copying from %s to %s\n", source, destination);
    return 0;
}
```

## Advanced Path Formats

When working with nested commands, cargs offers special path formats for accessing options:

### 1. Absolute Path

An absolute path specifies the full option path from the root:

```c
// Access option from anywhere 
const char* name = cargs_get(*cargs, "service.create.name").as_string;
```

### 2. Relative Path

Inside a subcommand action handler, you can use relative paths:

```c
int service_create_action(cargs_t *cargs, void *data) {
    // "name" automatically resolves to "service.create.name"
    const char* name = cargs_get(*cargs, "name").as_string;
    // ...
}
```

### 3. Root-Level Path

To access options defined at the root level from within a deeply nested subcommand:

```c
int service_create_action(cargs_t *cargs, void *data) {
    // Access root-level debug flag with dot prefix
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    // ...
}
```

## Detecting and Processing Nested Commands

### Checking Command Activation

To determine which commands are active at each level:

```c
// Check top-level command
if (cargs_is_set(cargs, "service")) {
    // Check second-level command
    if (cargs_is_set(cargs, "service.create")) {
        // "service create" was used
    }
} else if (cargs_is_set(cargs, "config")) {
    if (cargs_is_set(cargs, "config.set")) {
        // "config set" was used
    }
}
```

### Sharing Context Across Commands

The `void *data` parameter of `cargs_exec()` allows you to pass context to all command handlers:

```c
typedef struct {
    FILE *log_file;
    config_t *config;
} app_context_t;

int main(int argc, char **argv) {
    // Initialize context
    app_context_t context = {
        .log_file = fopen("app.log", "w"),
        .config = load_config()
    };
    
    // Normal initialization and parsing...
    
    if (cargs_has_command(cargs)) {
        // Pass context to command handlers
        status = cargs_exec(&cargs, &context);
    }
    
    // Clean up...
}

// Any command handler can access the context
int service_create_action(cargs_t *cargs, void *data) {
    app_context_t *context = (app_context_t *)data;
    fprintf(context->log_file, "Creating service...\n");
    // ...
}
```

## Creating Rich Command-Line Interfaces

With nested commands, you can create complex command structures like those found in modern CLI tools:

=== "Git-like Structure"
    ```c
    // Main options
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        VERSION_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("remote", remote_options, 
                   HELP("Manage remote repositories")),
        SUBCOMMAND("branch", branch_options,
                   HELP("Manage branches"))
    )
    
    // Remote subcommands
    CARGS_OPTIONS(
        remote_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("add", remote_add_options,
                   HELP("Add a remote"), 
                   ACTION(remote_add_action)),
        SUBCOMMAND("remove", remote_remove_options,
                   HELP("Remove a remote"),
                   ACTION(remote_remove_action))
    )
    ```

=== "Docker-like Structure"
    ```c
    // Main options
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        VERSION_OPTION(FLAGS(FLAG_EXIT)),
        
        // Global options for all commands
        OPTION_FLAG('q', "quiet", "Suppress output"),
        
        SUBCOMMAND("container", container_options,
                   HELP("Manage containers")),
        SUBCOMMAND("image", image_options,
                   HELP("Manage images"))
    )
    
    // Container subcommands
    CARGS_OPTIONS(
        container_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("run", container_run_options,
                   HELP("Run a container"),
                   ACTION(container_run_action)),
        SUBCOMMAND("stop", container_stop_options,
                   HELP("Stop a container"),
                   ACTION(container_stop_action))
    )
    ```

## Best Practices

1. **Limit the Nesting Depth**: Keep to 2-3 levels maximum for usability
2. **Be Consistent**: Use similar naming conventions across commands
3. **Place Global Options at the Root**: Options that apply across all commands should be at the top level
4. **Design for Abbreviation**: Ensure command names have distinct prefixes
5. **Document Command Structures**: Clearly document the command hierarchy in help text

## Complete Example

Here's a complete example demonstrating nested commands with abbreviation support:

```c
#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Action handlers
int service_create_action(cargs_t *cargs, void *data);
int service_list_action(cargs_t *cargs, void *data);
int config_set_action(cargs_t *cargs, void *data);
int config_get_action(cargs_t *cargs, void *data);

// Define options for "service create" command
CARGS_OPTIONS(
    service_create_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('n', "name", HELP("Service name"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", HELP("Container image"), FLAGS(FLAG_REQUIRED))
)

// Define options for "service list" command
CARGS_OPTIONS(
    service_list_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('a', "all", "Show all services, including stopped ones")
)

// Define options for the "service" parent command
CARGS_OPTIONS(
    service_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("create", service_create_options, 
               HELP("Create a new service"), 
               ACTION(service_create_action)),
    SUBCOMMAND("list", service_list_options, 
               HELP("List services"), 
               ACTION(service_list_action))
)

// Define options for "config set" command
CARGS_OPTIONS(
    config_set_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_STRING("key", "Configuration key"),
    POSITIONAL_STRING("value", "Configuration value")
)

// Define options for "config get" command
CARGS_OPTIONS(
    config_get_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_STRING("key", "Configuration key")
)

// Define options for the "config" parent command
CARGS_OPTIONS(
    config_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("set", config_set_options, 
               HELP("Set a configuration value"), 
               ACTION(config_set_action)),
    SUBCOMMAND("get", config_get_options, 
               HELP("Get a configuration value"), 
               ACTION(config_get_action))
)

// Define main options with top-level subcommands
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Global option at root level
    OPTION_FLAG('d', "debug", "Enable debug mode"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.log")),
    
    SUBCOMMAND("service", service_options, 
               HELP("Service management commands")),
    SUBCOMMAND("config", service_options, 
               HELP("Configuration commands"))
)

// Command action implementations
int service_create_action(cargs_t *cargs, void *data) {
    (void)data;
    
    // Different ways to access option values
    
    // 1. Relative path (relative to current subcommand)
    const char* name = cargs_get(*cargs, "name").as_string;
    const char* image = cargs_get(*cargs, "image").as_string;
    
    // 2. Absolute path (full path from root)
    const char* name_abs = cargs_get(*cargs, "service.create.name").as_string;
    (void)name_abs;
    
    // 3. Root-level path (access options at root level)
    const char* output = cargs_get(*cargs, ".output").as_string;
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    
    printf("Creating service '%s' using image '%s'\n", name, image);
    printf("Output file: %s\n", output);
    if (debug) printf("Debug mode enabled\n");
    
    return 0;
}

// Other action handlers...

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "nested_commands", "1.0.0");
    cargs.description = "Example of nested subcommands and command abbreviation";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    if (cargs_has_command(cargs)) {
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
```

## Limitations

- Maximum nesting depth is defined by `MAX_SUBCOMMAND_DEPTH` (default: 8)
- Command names cannot contain dots (`.`) as they are used as path separators
- Deeply nested commands can become unwieldy for users

!!! tip "Usability Considerations"
    While cargs supports deep command nesting, keeping your command structure 
    relatively flat with well-chosen command names often provides a better user experience.

## Related Resources

- [Subcommands Guide](../guide/subcommands.md) - Basic subcommand implementation
