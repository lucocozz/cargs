/**
 * Nested commands example for cargs
 * 
 * Demonstrates:
 * - Nested subcommands similar to complex CLI tools
 * - Path formats for accessing values (relative, absolute, root-level)
 */

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
    OPTION_STRING('n', "name", "Service name", FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", "Container image", FLAGS(FLAG_REQUIRED))
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
    SUBCOMMAND("config", config_options, 
               HELP("Configuration commands"))
)

// Command action implementations
int service_create_action(cargs_t *cargs, void *data) {
    (void)data;
    
    // Different ways to access option values
    
    // 1. Relative path (relative to current subcommand)
    // - When inside "service create" handler, "name" refers to "service.create.name"
    const char* name = cargs_get(*cargs, "name").as_string;
    const char* image = cargs_get(*cargs, "image").as_string;
    
    // 2. Absolute path (full path from root)
    // - Explicitly specifies the full path
    const char* name_abs = cargs_get(*cargs, "service.create.name").as_string;
    (void)name_abs;
    
    // 3. Root-level path (access options at root level)
    // - Starts with "." to force root level
    const char* output = cargs_get(*cargs, ".output").as_string;
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    
    printf("Creating service '%s' using image '%s'\n", name, image);
    printf("Output file: %s\n", output);
    if (debug) printf("Debug mode enabled\n");
    
    // Check if commands or options are set using cargs_is_set()
    printf("\nCommand check:\n");
    printf("- 'service' command is set: %s\n", 
           cargs_is_set(*cargs, "service") ? "yes" : "no");
    printf("- 'service.create' command is set: %s\n", 
           cargs_is_set(*cargs, "service.create") ? "yes" : "no");
    
    return 0;
}

int service_list_action(cargs_t *cargs, void *data) {
    (void)data;
    
    // Relative path (within current subcommand context)
    bool all = cargs_get(*cargs, "all").as_bool;
    
    // Root-level option
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    
    printf("Listing services (all=%s)\n", all ? "true" : "false");
    if (debug) printf("Debug mode enabled\n");
    
    // Demonstrating cargs_is_set with options
    printf("\nOption check:\n");
    printf("- 'all' option is set: %s\n", 
           cargs_is_set(*cargs, "all") ? "yes" : "no");
    printf("- Root-level 'debug' option is set: %s\n", 
           cargs_is_set(*cargs, ".debug") ? "yes" : "no");
    
    return 0;
}

int config_set_action(cargs_t *cargs, void *data) {
    (void)data;
    
    // Access positional arguments (relative path)
    const char* key = cargs_get(*cargs, "key").as_string;
    const char* value = cargs_get(*cargs, "value").as_string;
    
    // Alternative: absolute path
    const char* key_abs = cargs_get(*cargs, "config.set.key").as_string;
    (void)key_abs;
    
    // Check if positional arguments are set
    printf("Setting config '%s' to '%s'\n", key, value);
    printf("\nPositional check:\n");
    printf("- 'key' positional is set: %s\n", 
           cargs_is_set(*cargs, "key") ? "yes" : "no");
    printf("- 'value' positional is set: %s\n", 
           cargs_is_set(*cargs, "value") ? "yes" : "no");
    
    return 0;
}

int config_get_action(cargs_t *cargs, void *data) {
    (void)data;
    const char* key = cargs_get(*cargs, "config.get.key").as_string;
    
    printf("Getting config value for '%s'\n", key);
    return 0;
}

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "nested_commands", "1.0.0");
    cargs.description = "Example of nested subcommands and path formats";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Root-level options can be accessed directly from main
    bool debug = cargs_get(cargs, "debug").as_bool;
    if (debug) {
        printf("[Debug mode enabled at root level]\n");
    }
    
    // Check which subcommand was used with cargs_is_set
    if (cargs_is_set(cargs, "service")) {
        printf("Service command selected\n");
        
        // Check subcommands
        if (cargs_is_set(cargs, "service.create")) {
            printf("Service create subcommand selected\n");
        } else if (cargs_is_set(cargs, "service.list")) {
            printf("Service list subcommand selected\n");
        }
    } else if (cargs_is_set(cargs, "config")) {
        printf("Config command selected\n");
        
        // Check subcommands
        if (cargs_is_set(cargs, "config.set")) {
            printf("Config set subcommand selected\n");
        } else if (cargs_is_set(cargs, "config.get")) {
            printf("Config get subcommand selected\n");
        }
    }
    
    if (cargs_has_command(cargs)) {
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("No command specified. Use --help to see available commands.\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
