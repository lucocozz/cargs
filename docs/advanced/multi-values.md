# Multi-Value Collections

cargs provides powerful support for multi-value collections through arrays and maps, allowing you to handle complex command-line scenarios that traditional argument parsing libraries struggle with.

!!! abstract "Overview"
    This guide covers advanced techniques for working with collections:
    
    - **Array and Map Options** - Collecting multiple values and key-value pairs
    - **Iterator API** - Efficient traversal and filtering
    - **Collection Flags** - Sorting, uniqueness, and organization
    - **Performance Optimization** - Techniques for handling large collections
    - **Advanced Use Cases** - Real-world examples and patterns
    
    For basic usage, see the [Multi-Values guide](../guide/multi-values.md).

## Array Options

Array options allow users to provide multiple values for a single option, either through multiple occurrences or comma-separated lists.

### Supported Array Types

cargs supports these array types:

- `OPTION_ARRAY_STRING`: Array of strings
- `OPTION_ARRAY_INT`: Array of integers (with support for ranges)
- `OPTION_ARRAY_FLOAT`: Array of floating-point values

### Defining Array Options

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Simple string array without flags
    OPTION_ARRAY_STRING('n', "names", "List of names"),
    
    // Integer array with sorting and uniqueness
    OPTION_ARRAY_INT('i', "ids", "List of IDs",
                    FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    
    // Float array with custom hint
    OPTION_ARRAY_FLOAT('f', "factors", "Scaling factors",
                      HINT("FLOAT"))
)
```

### Providing Array Values

Users can provide array values in several ways:

=== "Multiple Option Occurrences"
    ```bash
    # Each option occurrence adds to the array
    ./program --names=john --names=alice --names=bob
    ```

=== "Comma-Separated Lists"
    ```bash
    # Comma-separated values
    ./program --names=john,alice,bob
    ```

=== "Mixed Approaches"
    ```bash
    # Combined approaches
    ./program --names=john,alice --names=bob
    ```

=== "Shorthand Syntax"
    ```bash
    # Short option variants
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

### Integer Range Syntax

For integer arrays, cargs supports a special range syntax:

```bash
# Ranges expand to include all values in the range
./program --ids=1-5,10,15-20
# Equivalent to: --ids=1,2,3,4,5,10,15,16,17,18,19,20
```

!!! tip "Range Handling"
    The range syntax is particularly useful for specifying port ranges, sequence IDs, or other numeric sequences without having to type each value individually.

## Map Options

Map options allow users to provide key-value pairs, enabling structured configuration through command-line arguments.

### Supported Map Types

cargs supports these map types:

| Type | Macro | Values | Example Usage |
|------|-------|--------|---------------|
| String Map | `OPTION_MAP_STRING` | Text strings | `--env=USER=alice,HOME=/home` |
| Integer Map | `OPTION_MAP_INT` | Integer numbers | `--ports=http=80,https=443` |
| Float Map | `OPTION_MAP_FLOAT` | Floating-point numbers | `--scales=width=0.5,height=1.5` |
| Boolean Map | `OPTION_MAP_BOOL` | Boolean values | `--features=debug=true,cache=false` |

### Boolean Value Handling

For boolean maps (`OPTION_MAP_BOOL`), values are parsed with the following conventions:

| True Values | False Values |
|-------------|--------------|
| "true", "yes", "1", "on", "y" | "false", "no", "0", "off", "n" |

All values are case-insensitive, so "True", "TRUE", and "true" are all equivalent.

## Collection Flags

cargs provides special flags to modify how collections are processed:

### Array Flags

| Flag | Description | Example |
|------|-------------|---------|
| `FLAG_SORTED` | Sort array values | `OPTION_ARRAY_STRING('t', "tags", "Tags", FLAGS(FLAG_SORTED))` |
| `FLAG_UNIQUE` | Remove duplicate values | `OPTION_ARRAY_INT('p', "ports", "Ports", FLAGS(FLAG_UNIQUE))` |

When both are used together, duplicates are removed after sorting:

```c
OPTION_ARRAY_INT('i', "ids", "IDs", FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

### Map Flags

| Flag | Description | Example |
|------|-------------|---------|
| `FLAG_SORTED_KEY` | Sort map entries by key | `FLAGS(FLAG_SORTED_KEY)` |
| `FLAG_SORTED_VALUE` | Sort map entries by value | `FLAGS(FLAG_SORTED_VALUE)` |
| `FLAG_UNIQUE_VALUE` | Ensure no duplicate values | `FLAGS(FLAG_UNIQUE_VALUE)` |

!!! warning "Flag Precedence"
    When both `FLAG_SORTED_KEY` and `FLAG_SORTED_VALUE` are specified, `FLAG_SORTED_KEY` takes precedence.

## Accessing Collections

cargs provides multiple ways to access collection data, each with its own advantages:

### Direct Access

The most straightforward approach is direct access to the value arrays or maps:

```c
// Get the entire array
value_t *names_array = cargs_get(cargs, "names").as_array;
size_t names_count = cargs_count(cargs, "names");

// Access array elements directly
for (size_t i = 0; i < names_count; i++) {
    const char* name = names_array[i].as_string;
    printf("Name %zu: %s\n", i + 1, name);
}

// Get the entire map
cargs_pair_t *env_map = cargs_get(cargs, "env").as_map;
size_t env_count = cargs_count(cargs, "env");

// Access map entries directly
for (size_t i = 0; i < env_count; i++) {
    const char* key = env_map[i].key;
    const char* value = env_map[i].value.as_string;
    printf("%s = %s\n", key, value);
}
```

### Element Access Helpers

cargs provides helper functions for more convenient access to specific elements:

```c
// Get a specific array element by index
const char* first_name = cargs_array_get(cargs, "names", 0).as_string;
int second_id = cargs_array_get(cargs, "ids", 1).as_int;

// Look up a specific map value by key
const char* user = cargs_map_get(cargs, "env", "USER").as_string;
int http_port = cargs_map_get(cargs, "ports", "http").as_int;
bool debug_enabled = cargs_map_get(cargs, "features", "debug").as_bool;
```

These helper functions handle invalid indices or missing keys gracefully, returning an empty value (`{.raw = 0}`) when the requested element doesn't exist.

### Iterator API

For more idiomatic iteration, cargs provides a clean iterator API:

#### Array Iterators

```c
// Create an array iterator
cargs_array_it_t names_it = cargs_array_it(cargs, "names");

// Iterate through all elements
while (cargs_array_next(&names_it)) {
    printf("Name: %s\n", names_it.value.as_string);
}
```

#### Map Iterators

```c
// Create a map iterator
cargs_map_it_t env_it = cargs_map_it(cargs, "env");

// Iterate through all key-value pairs
while (cargs_map_next(&env_it)) {
    printf("%s = %s\n", env_it.key, env_it.value.as_string);
}
```

#### Iterator Reset

Iterators can be reset and reused for multiple passes through the collection:

```c
cargs_map_it_t features_it = cargs_map_it(cargs, "features");

// First pass: print enabled features
printf("Enabled features: ");
while (cargs_map_next(&features_it)) {
    if (features_it.value.as_bool) {
        printf("%s ", features_it.key);
    }
}
printf("\n");

// Reset the iterator for a second pass
cargs_map_reset(&features_it);

// Second pass: print disabled features
printf("Disabled features: ");
while (cargs_map_next(&features_it)) {
    if (!features_it.value.as_bool) {
        printf("%s ", features_it.key);
    }
}
printf("\n");
```

!!! tip "Filtering with Iterators"
    Iterators are especially useful when you need to filter or categorize elements based on their values, as shown in the example above.

## Performance Considerations

When working with large collections, consider these performance tips:

### 1. Choose the Right Access Method

Different access methods have different performance characteristics:

| Access Method | Best For | Performance Characteristics |
|---------------|----------|----------------------------|
| Direct Access | Random access to elements | O(1) access but requires managing indices |
| Helper Functions | Looking up specific elements | O(1) for arrays, O(n) for maps |
| Iterators | Sequential processing of all elements | Most efficient for complete traversal |

### 2. Collection Processing Flags

Collection flags add processing overhead:

- `FLAG_SORTED`: Requires O(n log n) sorting time
- `FLAG_UNIQUE`: Requires O(n²) comparison time for naive implementation, O(n log n) with sorting
- `FLAG_SORTED_KEY` / `FLAG_SORTED_VALUE`: Requires O(n log n) sorting time

Only use these flags when the benefits outweigh the processing cost.

### 3. Memory Management

Collections are dynamically allocated with an initial capacity of 8 elements, doubling in size when needed:

```c
#define MULTI_VALUE_INITIAL_CAPACITY 8
```

For extremely large collections, consider using a custom handler that preallocates the appropriate capacity.

## Advanced Use Cases

### Command-Line Tags or Labels

```c
OPTION_ARRAY_STRING('t', "tag", "Tags for the resource",
                   FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

Usage:
```bash
./program --tag=important,urgent --tag=priority
```

### Environment Variable Overrides

```c
OPTION_MAP_STRING('D', "define", "Define configuration variables")
```

Usage:
```bash
./program -Ddebug=true -Dlog.level=info -Dserver.port=8080
```

### Feature Toggle Management

A complete example of feature management with toggles:

```c
#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Feature flags with sorted keys for consistent display
    OPTION_MAP_BOOL('f', "feature", "Feature flags",
                   FLAGS(FLAG_SORTED_KEY))
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "feature_manager", "1.0.0");
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    if (cargs_is_set(cargs, "feature")) {
        // Create categories for enabled/disabled
        printf("Feature Configuration:\n");
        
        printf("  Enabled Features:\n");
        cargs_map_it_t it = cargs_map_it(cargs, "feature");
        while (cargs_map_next(&it)) {
            if (it.value.as_bool) {
                printf("    - %s\n", it.key);
            }
        }
        
        printf("  Disabled Features:\n");
        cargs_map_reset(&it);
        while (cargs_map_next(&it)) {
            if (!it.value.as_bool) {
                printf("    - %s\n", it.key);
            }
        }
    } else {
        printf("No features configured.\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
```

## Technical Implementation Details

Behind the scenes, cargs implements collections using efficient data structures:

### Arrays

Arrays are implemented as dynamic arrays of `value_t` elements:

```c
// Array storage in option
option->value.as_array = malloc(option->value_capacity * sizeof(value_t));
```

When an array needs to grow:

```c
void adjust_array_size(cargs_option_t *option)
{
    if (option->value.as_array == NULL) {
        option->value_capacity = MULTI_VALUE_INITIAL_CAPACITY;
        option->value.as_array = malloc(option->value_capacity * sizeof(value_t));
    } else if (option->value_count >= option->value_capacity) {
        option->value_capacity *= 2;
        void *new = realloc(option->value.as_array, option->value_capacity * sizeof(value_t));
        if (new == NULL) {
            option->value_capacity /= 2;
            return;
        }
        option->value.as_array = new;
    }
}
```

### Maps

Maps are implemented as dynamic arrays of `cargs_pair_t` elements:

```c
typedef struct cargs_pair_s
{
    const char *key;
    value_t     value;
} cargs_pair_t;
```

Key lookup is performed by linear search:

```c
int map_find_key(cargs_option_t *option, const char *key)
{
    for (size_t i = 0; i < option->value_count; ++i) {
        if (option->value.as_map[i].key && strcmp(option->value.as_map[i].key, key) == 0)
            return ((int)i);
    }
    return (-1);
}
```

### Iterator Implementation

Iterators are simple structures that maintain a reference to the collection and a current position:

```c
typedef struct cargs_array_iterator_s
{
    value_t *_array;    /* Pointer to the array */
    size_t   _count;    /* Number of elements */
    size_t   _position; /* Current position */
    value_t  value;     /* Current value */
} cargs_array_it_t;

typedef struct cargs_map_iterator_s
{
    cargs_pair_t *_map;      /* Pointer to the map */
    size_t        _count;    /* Number of elements */
    size_t        _position; /* Current position */
    const char   *key;       /* Current key */
    value_t       value;     /* Current value */
} cargs_map_it_t;
```

## Complete Example

Here's a complete example demonstrating advanced collection handling techniques:

```c
#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Array options with flags
    OPTION_ARRAY_STRING('n', "name", "Names of users",
                       FLAGS(FLAG_SORTED)),
    OPTION_ARRAY_INT('i', "id", "User IDs",
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    
    // Map options with flags
    OPTION_MAP_STRING('e', "env", "Environment variables",
                     FLAGS(FLAG_SORTED_KEY)),
    OPTION_MAP_INT('p', "port", "Port mappings",
                  FLAGS(FLAG_SORTED_KEY)),
    OPTION_MAP_BOOL('f', "feature", "Feature flags")
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "multi_values", "1.0.0");
    cargs.description = "Advanced multi-value collections example";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Process arrays using iterators
    if (cargs_is_set(cargs, "name")) {
        printf("Users:\n");
        cargs_array_it_t it = cargs_array_it(cargs, "name");
        while (cargs_array_next(&it)) {
            printf("  - %s\n", it.value.as_string);
        }
    }
    
    if (cargs_is_set(cargs, "id")) {
        printf("User IDs:\n");
        cargs_array_it_t it = cargs_array_it(cargs, "id");
        while (cargs_array_next(&it)) {
            printf("  - %d\n", it.value.as_int);
        }
    }
    
    // Process maps using iterators
    if (cargs_is_set(cargs, "env")) {
        printf("Environment Variables:\n");
        cargs_map_it_t it = cargs_map_it(cargs, "env");
        while (cargs_map_next(&it)) {
            printf("  %s = %s\n", it.key, it.value.as_string);
        }
    }
    
    if (cargs_is_set(cargs, "port")) {
        printf("Port Mappings:\n");
        cargs_map_it_t it = cargs_map_it(cargs, "port");
        while (cargs_map_next(&it)) {
            printf("  %s: %d\n", it.key, it.value.as_int);
        }
    }
    
    // Process boolean map with categories and filtering
    if (cargs_is_set(cargs, "feature")) {
        printf("Features:\n");
        
        printf("  Enabled:");
        cargs_map_it_t it = cargs_map_it(cargs, "feature");
        while (cargs_map_next(&it)) {
            if (it.value.as_bool) {
                printf(" %s", it.key);
            }
        }
        printf("\n");
        
        printf("  Disabled:");
        cargs_map_reset(&it);
        while (cargs_map_next(&it)) {
            if (!it.value.as_bool) {
                printf(" %s", it.key);
            }
        }
        printf("\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
```

This example demonstrates:

1. Array and map options with sorting and uniqueness flags
2. Iterator-based processing for all collections
3. Filtering and categorization using iterators
4. Proper resource management

## Summary

cargs' multi-value collections provide a powerful way to handle complex command-line interfaces with:

- **Multiple values** for a single option (arrays)
- **Key-value configuration** through a single option (maps)
- **Flexible input formats** for user convenience
- **Sorting and uniqueness** for organized data
- **Efficient iteration** through the iterator API

These advanced features allow cargs to handle scenarios that would be difficult or impossible with traditional argument parsing libraries.
