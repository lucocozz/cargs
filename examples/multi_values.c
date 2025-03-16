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
    OPTION_INT('a', "approach", "Processing approach: 1=direct, 2=element, 3=iterator", 
               DEFAULT(1), RANGE(1, 3)),
    
    // Array options with custom HINT()
    OPTION_ARRAY_STRING('n', "names", "Array of user names (comma-separated)",
                       HINT("NAME"),
                       FLAGS(FLAG_SORTED)),
                       
    OPTION_ARRAY_INT('i', "ids", "Array of ID numbers or ranges (e.g., 1,2,3-5)",
                    HINT("ID"),
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    
    // Map options with custom HINT()
    OPTION_MAP_STRING('\0', "env", "Environment variables (key=value pairs)",
                    HINT("VALUE"),
                    FLAGS(FLAG_SORTED_KEY)),
                     
    OPTION_MAP_INT('\0', "ports", "Port numbers for services",
                    HINT("PORT"),
                    FLAGS(FLAG_UNIQUE_VALUE)),
                  
    OPTION_MAP_FLOAT('\0', "scales", "Scaling factors for dimensions",
                    HINT("FACTOR"),
                    FLAGS(FLAG_SORTED_VALUE)),
                    
    OPTION_MAP_BOOL('\0', "flags", "Feature flags configuration",
                   HINT("FLAG"))
)

// Process arrays using direct approach (original)
void process_arrays_direct(cargs_t cargs) {
    printf("=== APPROACH 1: DIRECT ACCESS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names")) {
        value_t *names = cargs_get_value(cargs, "names").as_array;
        size_t count = cargs_get_value_count(cargs, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: \"%s\"\n", i, names[i].as_string);
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids")) {
        value_t *ids = cargs_get_value(cargs, "ids").as_array;
        size_t count = cargs_get_value_count(cargs, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: %d\n", i, ids[i].as_int);
        printf("\n");
    }
}

// Process maps using direct approach (original)
void process_maps_direct(cargs_t cargs) {
    // Handle map of strings
    if (cargs_is_set(cargs, "env")) {
        cargs_pair_t *env = cargs_get_value(cargs, "env").as_map;
        size_t count = cargs_get_value_count(cargs, "env");
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => '%s'\n", 
                   env[i].key, 
                   env[i].value.as_string);
        }
        printf("\n");
    }

    // Handle map of integers
    if (cargs_is_set(cargs, "ports")) {
        cargs_pair_t *ports = cargs_get_value(cargs, "ports").as_map;
        size_t count = cargs_get_value_count(cargs, "ports");
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %ld\n", 
                   ports[i].key, 
                   ports[i].value.as_int64);
        }
        printf("\n");
    }

    // Handle map of floats
    if (cargs_is_set(cargs, "scales")) {
        cargs_pair_t *scales = cargs_get_value(cargs, "scales").as_map;
        size_t count = cargs_get_value_count(cargs, "scales");
        
        printf("Scaling factors (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %.3f\n", 
                   scales[i].key, 
                   scales[i].value.as_float);
        }
        printf("\n");
    }

    // Handle map of booleans
    if (cargs_is_set(cargs, "flags")) {
        cargs_pair_t *flags = cargs_get_value(cargs, "flags").as_map;
        size_t count = cargs_get_value_count(cargs, "flags");
        
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
void process_arrays_element(cargs_t cargs) {
    printf("=== APPROACH 2: ELEMENT ACCESS HELPERS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names")) {
        size_t count = cargs_get_value_count(cargs, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char* name = cargs_get_array_element(cargs, "names", i).as_string;
            printf("  [%zu]: \"%s\"\n", i, name);
        }
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids")) {
        size_t count = cargs_get_value_count(cargs, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            int id = cargs_get_array_element(cargs, "ids", i).as_int;
            printf("  [%zu]: %d\n", i, id);
        }
        printf("\n");
    }
}

// Process maps using element access helpers
void process_maps_element(cargs_t cargs) {
    // First show direct key access examples

    // Handle map of strings
    if (cargs_is_set(cargs, "env")) {
        printf("Direct key lookups:\n");
        
        const char* user = cargs_get_map_value(cargs, "env", "USER").as_string;
        if (user) {
            printf("  USER = %s\n", user);
        }
        
        const char* home = cargs_get_map_value(cargs, "env", "HOME").as_string;
        if (home) {
            printf("  HOME = %s\n", home);
        }
        printf("\n");
    }

    // Handle map of integers with direct key access
    if (cargs_is_set(cargs, "ports")) {
        printf("Common port lookups:\n");
        
        int http = cargs_get_map_value(cargs, "ports", "http").as_int;
        if (http) {
            printf("  HTTP port: %d\n", http);
        }
        
        int https = cargs_get_map_value(cargs, "ports", "https").as_int;
        if (https) {
            printf("  HTTPS port: %d\n", https);
        }
        printf("\n");
    }

    // Now show complete listings using regular iteration with map value helper

    // Handle map of strings
    if (cargs_is_set(cargs, "env")) {
        size_t count = cargs_get_value_count(cargs, "env");
        cargs_pair_t *env = cargs_get_value(cargs, "env").as_map;
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = env[i].key;
            const char *value = cargs_get_map_value(cargs, "env", key).as_string;
            printf("  '%s' => '%s'\n", key, value);
        }
        printf("\n");
    }

    // Handle map of integers (similarly for other maps)
    if (cargs_is_set(cargs, "ports")) {
        size_t count = cargs_get_value_count(cargs, "ports");
        cargs_pair_t *ports = cargs_get_value(cargs, "ports").as_map;
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = ports[i].key;
            int value = cargs_get_map_value(cargs, "ports", key).as_int;
            printf("  '%s' => %d\n", key, value);
        }
        printf("\n");
    }
}

// Process arrays using iterator approach
void process_arrays_iterator(cargs_t cargs) {
    printf("=== APPROACH 3: ITERATORS ===\n\n");
    
    // Handle array of strings
    if (cargs_is_set(cargs, "names")) {
        cargs_array_iterator_t it = cargs_array_iterator(cargs, "names");
        value_t value;
        int i = 0;
        
        printf("Names array (%zu items):\n", it.count);
        while (cargs_array_next(&it, &value)) {
            printf("  [%d]: \"%s\"\n", i++, value.as_string);
        }
        printf("\n");
    }

    // Handle array of integers
    if (cargs_is_set(cargs, "ids")) {
        cargs_array_iterator_t it = cargs_array_iterator(cargs, "ids");
        value_t value;
        int i = 0;
        
        printf("ID numbers array (%zu items):\n", it.count);
        while (cargs_array_next(&it, &value)) {
            printf("  [%d]: %d\n", i++, value.as_int);
        }
        printf("\n");
    }
}

// Process maps using iterator approach
void process_maps_iterator(cargs_t cargs) {
    // Handle map of strings
    if (cargs_is_set(cargs, "env")) {
        cargs_map_iterator_t it = cargs_map_iterator(cargs, "env");
        const char *key;
        value_t value;
        
        printf("Environment variables (%zu items):\n", it.count);
        while (cargs_map_next(&it, &key, &value)) {
            printf("  '%s' => '%s'\n", key, value.as_string);
        }
        printf("\n");
    }

    // Handle map of integers
    if (cargs_is_set(cargs, "ports")) {
        cargs_map_iterator_t it = cargs_map_iterator(cargs, "ports");
        const char *key;
        value_t value;
        
        printf("Port mappings (%zu items):\n", it.count);
        while (cargs_map_next(&it, &key, &value)) {
            printf("  '%s' => %ld\n", key, value.as_int64);
        }
        printf("\n");
    }

    // Handle map of floats
    if (cargs_is_set(cargs, "scales")) {
        cargs_map_iterator_t it = cargs_map_iterator(cargs, "scales");
        const char *key;
        value_t value;
        
        printf("Scaling factors (%zu items):\n", it.count);
        while (cargs_map_next(&it, &key, &value)) {
            printf("  '%s' => %.3f\n", key, value.as_float);
        }
        printf("\n");
    }

    // Handle map of booleans
    if (cargs_is_set(cargs, "flags")) {
        cargs_map_iterator_t it = cargs_map_iterator(cargs, "flags");
        const char *key;
        value_t value;
        
        printf("Feature flags (%zu items):\n", it.count);
        while (cargs_map_next(&it, &key, &value)) {
            printf("  '%s' => %s\n", key, value.as_bool ? "enabled" : "disabled");
        }
        printf("\n");
    }

    // Example of iterator re-use for multiple scans
    if (cargs_is_set(cargs, "flags")) {
        cargs_map_iterator_t it = cargs_map_iterator(cargs, "flags");
        const char *key;
        value_t value;
        
        printf("Enabled features only:\n");
        while (cargs_map_next(&it, &key, &value)) {
            if (value.as_bool) {
                printf("  '%s'\n", key);
            }
        }
        
        printf("\nDisabled features only:\n");
        cargs_map_reset(&it);  // Reset the iterator for second pass
        while (cargs_map_next(&it, &key, &value)) {
            if (!value.as_bool) {
                printf("  '%s'\n", key);
            }
        }
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
    int approach = cargs_get_value(cargs, "approach").as_int;
    
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
