/**
 * Multi-values example for cargs
 * 
 * Demonstrates different approaches to working with array and map data structures:
 * 1. Direct access (original approach)
 * 2. Element access helpers
 * 3. Iterator approach
 * 
 * Examples:
 * - Arrays:
 *   ./multi_values --names="john,alice,bob" --ids="1,2,3-5,10"
 * 
 * - Maps (different types):
 *   ./multi_values --env="name=value,host=localhost" 
 *   ./multi_values --ports="http=80,https=443,ftp=21"
 *   ./multi_values --scales="width=0.5,height=1.5,zoom=2.0"
 *   ./multi_values --flags="debug=true,verbose=yes,logging=on,cache=false"
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Define options
CARGS_OPTIONS(
    options,
    // Standard options
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Demo options to select processing approach
    OPTION_INT('a', "approach", HELP("Processing approach: 1=direct, 2=element, 3=iterator"), 
               DEFAULT(1), RANGE(1, 3)),
    
    // Array options with custom HINT()
    OPTION_ARRAY_STRING('n', "names", HELP("Array of user names (comma-separated)"),
                       HINT("NAME"),
                       FLAGS(FLAG_SORTED)),
                       
    OPTION_ARRAY_INT('i', "ids", HELP("Array of ID numbers or ranges (e.g., 1,2,3-5)"),
                    HINT("ID"),
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    
    // Map options with custom HINT()
    OPTION_MAP_STRING('\0', "env", HELP("Environment variables (key=value pairs)"),
                    HINT("VALUE"),
                    FLAGS(FLAG_SORTED_KEY)),
                     
    OPTION_MAP_INT('\0', "ports", HELP("Port numbers for services"),
                    HINT("PORT"),
                    FLAGS(FLAG_UNIQUE_VALUE)),
                  
    OPTION_MAP_FLOAT('\0', "scales", HELP("Scaling factors for dimensions"),
                    HINT("FACTOR"),
                    FLAGS(FLAG_SORTED_VALUE)),
                    
    OPTION_MAP_BOOL('\0', "flags", HELP("Feature flags configuration"),
                    HINT("FLAG"))
)

// Process arrays using direct approach (original)
void process_arrays_direct(cargs_t cargs)
{
    printf("=== APPROACH 1: DIRECT ACCESS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names"))
    {
        cargs_value_t *names = cargs_get(cargs, "names").as_array;
        size_t count = cargs_count(cargs, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: \"%s\"\n", i, names[i].as_string);
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids"))
    {
        cargs_value_t *ids = cargs_get(cargs, "ids").as_array;
        size_t count = cargs_count(cargs, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: %d\n", i, ids[i].as_int);
        printf("\n");
    }
}

// Process maps using direct approach (original)
void process_maps_direct(cargs_t cargs)
{
    // Handle map of strings
    if (cargs_is_set(cargs, "env"))
    {
        cargs_pair_t *env = cargs_get(cargs, "env").as_map;
        size_t count = cargs_count(cargs, "env");
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => '%s'\n", 
                   env[i].key, 
                   env[i].value.as_string);
        }
        printf("\n");
    }

    // Handle map of integers
    if (cargs_is_set(cargs, "ports"))
    {
        cargs_pair_t *ports = cargs_get(cargs, "ports").as_map;
        size_t count = cargs_count(cargs, "ports");
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %d\n", 
                   ports[i].key, 
                   ports[i].value.as_int);
        }
        printf("\n");
    }

    // Handle map of floats
    if (cargs_is_set(cargs, "scales"))
    {
        cargs_pair_t *scales = cargs_get(cargs, "scales").as_map;
        size_t count = cargs_count(cargs, "scales");
        
        printf("Scaling factors (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %.3f\n", 
                   scales[i].key, 
                   scales[i].value.as_float);
        }
        printf("\n");
    }

    // Handle map of booleans
    if (cargs_is_set(cargs, "flags"))
    {
        cargs_pair_t *flags = cargs_get(cargs, "flags").as_map;
        size_t count = cargs_count(cargs, "flags");
        
        printf("Feature flags (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %s\n", 
                   flags[i].key, 
                   flags[i].value.as_bool ? "enabled" : "disabled");
        }
        printf("\n");
    }
}

// Process arrays using element access helpers
void process_arrays_element(cargs_t cargs)
{
    printf("=== APPROACH 2: ELEMENT ACCESS HELPERS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names")) {
        size_t count = cargs_count(cargs, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char* name = cargs_array_get(cargs, "names", i).as_string;
            printf("  [%zu]: \"%s\"\n", i, name);
        }
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids"))
    {
        size_t count = cargs_count(cargs, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            int id = cargs_array_get(cargs, "ids", i).as_int;
            printf("  [%zu]: %d\n", i, id);
        }
        printf("\n");
    }
}

// Process maps using element access helpers
void process_maps_element(cargs_t cargs)
{
    // First show direct key access examples

    // Handle map of strings
    if (cargs_is_set(cargs, "env"))
    {
        printf("Direct key lookups:\n");
        
        const char* user = cargs_map_get(cargs, "env", "USER").as_string;
        if (user)
            printf("  USER = %s\n", user);

        const char* home = cargs_map_get(cargs, "env", "HOME").as_string;
        if (home)
            printf("  HOME = %s\n", home);
        printf("\n");
    }

    // Handle map of integers with direct key access
    if (cargs_is_set(cargs, "ports"))
    {
        printf("Common port lookups:\n");
        
        int http = cargs_map_get(cargs, "ports", "http").as_int;
        if (http)
            printf("  HTTP port: %d\n", http);

        int https = cargs_map_get(cargs, "ports", "https").as_int;
        if (https)
            printf("  HTTPS port: %d\n", https);
        printf("\n");
    }

    // Now show complete listings using regular iteration with map value helper

    // Handle map of strings
    if (cargs_is_set(cargs, "env"))
    {
        size_t count = cargs_count(cargs, "env");
        cargs_pair_t *env = cargs_get(cargs, "env").as_map;
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = env[i].key;
            const char *value = cargs_map_get(cargs, "env", key).as_string;
            printf("  '%s' => '%s'\n", key, value);
        }
        printf("\n");
    }

    // Handle map of integers (similarly for other maps)
    if (cargs_is_set(cargs, "ports"))
    {
        size_t count = cargs_count(cargs, "ports");
        cargs_pair_t *ports = cargs_get(cargs, "ports").as_map;
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = ports[i].key;
            int value = cargs_map_get(cargs, "ports", key).as_int;
            printf("  '%s' => %d\n", key, value);
        }
        printf("\n");
    }
}

// Process arrays using iterator approach
void process_arrays_iterator(cargs_t cargs) {
    printf("=== APPROACH 3: ITERATORS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names"))
    {
        cargs_array_it_t it = cargs_array_it(cargs, "names");

        printf("Names array (%zu items):\n", it._count);
        for (int i = 0; cargs_array_next(&it); i++)
            printf("  [%d]: \"%s\"\n", i++, it.value.as_string);
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids"))
    {
        cargs_array_it_t it = cargs_array_it(cargs, "ids");

        printf("ID numbers array (%zu items):\n", it._count);
        for (int i = 0; cargs_array_next(&it); i++)
            printf("  [%d]: %d\n", i++, it.value.as_int);
        printf("\n");
    }
}

// Process maps using iterator approach
void process_maps_iterator(cargs_t cargs)
{
    // Handle map of strings
    if (cargs_is_set(cargs, "env"))
    {
        cargs_map_it_t it = cargs_map_it(cargs, "env");

        printf("Environment variables (%zu items):\n", it._count);
        while (cargs_map_next(&it))
            printf("  '%s' => '%s'\n", it.key, it.value.as_string);
        printf("\n");
    }

    // Handle map of integers
    if (cargs_is_set(cargs, "ports"))
    {
        cargs_map_it_t it = cargs_map_it(cargs, "ports");

        printf("Port mappings (%zu items):\n", it._count);
        while (cargs_map_next(&it))
            printf("  '%s' => %d\n", it.key, it.value.as_int);
        printf("\n");
    }

    // Handle map of floats
    if (cargs_is_set(cargs, "scales"))
    {
        cargs_map_it_t it = cargs_map_it(cargs, "scales");

        printf("Scaling factors (%zu items):\n", it._count);
        while (cargs_map_next(&it))
            printf("  '%s' => %.3f\n", it.key, it.value.as_float);
        printf("\n");
    }

    // Handle map of booleans
    if (cargs_is_set(cargs, "flags"))
    {
        cargs_map_it_t it = cargs_map_it(cargs, "flags");

        printf("Feature flags (%zu items):\n", it._count);
        while (cargs_map_next(&it))
            printf("  '%s' => %s\n", it.key, it.value.as_bool ? "enabled" : "disabled");
        printf("\n");
    }

    // Example of iterator re-use for multiple scans
    if (cargs_is_set(cargs, "flags"))
    {
        cargs_map_it_t it = cargs_map_it(cargs, "flags");

        printf("Enabled features only:\n");
        while (cargs_map_next(&it))
            if (it.value.as_bool)
                printf("  '%s'\n", it.key);

        printf("\nDisabled features only:\n");
        cargs_map_reset(&it);  // Reset the iterator for second pass
        while (cargs_map_next(&it))
            if (!it.value.as_bool)
                printf("  '%s'\n", it.key);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "multi_values", "1.0.0");
    cargs.description = "Example of multi-value options with different access approaches";

    // Parse command-line arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS)
        return status;

    // Get the selected approach
    int approach = cargs_get(cargs, "approach").as_int;
    
    // Process data using the selected approach
    switch (approach) {
        case 1:
            process_arrays_direct(cargs);
            process_maps_direct(cargs);
            break;
        case 2:
            process_arrays_element(cargs);
            process_maps_element(cargs);
            break;
        case 3:
            process_arrays_iterator(cargs);
            process_maps_iterator(cargs);
            break;
        default:
            printf("Invalid approach selected. Please use 1, 2, or 3.\n");
            break;
    }

    // Free resources
    cargs_free(&cargs);
    return 0;
}
