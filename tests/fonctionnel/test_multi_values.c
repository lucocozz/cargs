#include <criterion/criterion.h>
#include "cargs.h"
#include <stdlib.h>
#include <string.h>

// Test options with array and map values
CARGS_OPTIONS(
    multi_value_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_ARRAY_STRING('s', "strings", HELP("Array of strings")),
    OPTION_ARRAY_INT('i', "ints", HELP("Array of integers")),
    OPTION_MAP_STRING('m', "map", HELP("String map")),
    OPTION_MAP_INT('p', "ports", HELP("Port map"))
)

// Setup function that parses arguments with multi-value options
static cargs_t setup_multi_value_cargs(void)
{
    char *argv[] = {
        "test_program",
        "--strings=one", "--strings", "two", "-sthree",
        "--ints=1,2,3,4,5",
        "--map=key1=value1", "--map", "key2=value2", "-mkey3=value3",
        "--ports=http=80,https=443,smtp=25"
    };
    int argc = sizeof(argv) / sizeof(char *);

    cargs_t cargs = cargs_init(multi_value_options, "test_program", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);

    cr_assert_eq(status, CARGS_SUCCESS, "Parsing should succeed");
    
    return cargs;
}

// Test cargs_array_get function
Test(multi_value_access, cargs_array_get)
{
    cargs_t cargs = setup_multi_value_cargs();
    
    // Check string array values
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 0).as_string, "one", "First string should be 'one'");
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 1).as_string, "two", "Second string should be 'two'");
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 2).as_string, "three", "Third string should be 'three'");
    
    // Check int array values
    cr_assert_eq(cargs_array_get(cargs, "ints", 0).as_int, 1, "First int should be 1");
    cr_assert_eq(cargs_array_get(cargs, "ints", 2).as_int, 3, "Third int should be 3");
    cr_assert_eq(cargs_array_get(cargs, "ints", 4).as_int, 5, "Fifth int should be 5");
    
    // Check out-of-bounds access
    cr_assert_eq(cargs_array_get(cargs, "strings", 10).raw, 0, "Out-of-bounds access should return empty value");
    
    // Check non-existent option
    cr_assert_eq(cargs_array_get(cargs, "nonexistent", 0).raw, 0, "Non-existent option should return empty value");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_map_get function
Test(multi_value_access, cargs_map_get)
{
    cargs_t cargs = setup_multi_value_cargs();
    
    // Check string map values
    cr_assert_str_eq(cargs_map_get(cargs, "map", "key1").as_string, "value1", "key1 should map to 'value1'");
    cr_assert_str_eq(cargs_map_get(cargs, "map", "key2").as_string, "value2", "key2 should map to 'value2'");
    cr_assert_str_eq(cargs_map_get(cargs, "map", "key3").as_string, "value3", "key3 should map to 'value3'");
    
    // Check int map values
    cr_assert_eq(cargs_map_get(cargs, "ports", "http").as_int, 80, "http should map to 80");
    cr_assert_eq(cargs_map_get(cargs, "ports", "https").as_int, 443, "https should map to 443");
    cr_assert_eq(cargs_map_get(cargs, "ports", "smtp").as_int, 25, "smtp should map to 25");
    
    // Check non-existent key
    cr_assert_eq(cargs_map_get(cargs, "map", "nonexistent").raw, 0, "Non-existent key should return empty value");
    
    // Check non-existent option
    cr_assert_eq(cargs_map_get(cargs, "nonexistent", "key").raw, 0, "Non-existent option should return empty value");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_array_it iterator
Test(multi_value_access, cargs_array_it)
{
    cargs_t cargs = setup_multi_value_cargs();
    
    // Test string array iterator
    cargs_array_it_t str_it = cargs_array_it(cargs, "strings");
    
    // Check iterator metadata
    cr_assert_eq(str_it._count, 3, "String array should have 3 elements");
    
    // Iterate over string array
    int str_count = 0;
    const char* expected_strings[] = {"one", "two", "three"};
    
    while (cargs_array_next(&str_it)) {
        cr_assert_str_eq(str_it.value.as_string, expected_strings[str_count], 
                         "String value should match expected");
        str_count++;
    }
    
    cr_assert_eq(str_count, 3, "Iterator should yield 3 values");
    
    // Test int array iterator
    cargs_array_it_t int_it = cargs_array_it(cargs, "ints");
    
    // Check iterator metadata
    cr_assert_eq(int_it._count, 5, "Int array should have 5 elements");
    
    // Iterate over int array
    int int_count = 0;
    int expected_ints[] = {1, 2, 3, 4, 5};
    
    while (cargs_array_next(&int_it)) {
        cr_assert_eq(int_it.value.as_int, expected_ints[int_count], 
                     "Int value should match expected");
        int_count++;
    }
    
    cr_assert_eq(int_count, 5, "Iterator should yield 5 values");
    
    // Test reset functionality
    cargs_array_reset(&int_it);
    cr_assert_eq(int_it._position, 0, "Reset should set position to 0");
    
    // Verify we can iterate again
    cr_assert(cargs_array_next(&int_it), "Iterator should yield values after reset");
    cr_assert_eq(int_it.value.as_int, 1, "First value after reset should be 1");
    
    // Clean up
    cargs_free(&cargs);
}

// Test cargs_map_it iterator
Test(multi_value_access, cargs_map_it)
{
    cargs_t cargs = setup_multi_value_cargs();
    
    // Test string map iterator
    cargs_map_it_t map_it = cargs_map_it(cargs, "map");
    
    // Check iterator metadata
    cr_assert_eq(map_it._count, 3, "String map should have 3 elements");
    
    // Iterate over string map
    int map_count = 0;
    bool found_key1 = false, found_key2 = false, found_key3 = false;
    
    while (cargs_map_next(&map_it))
    {
        map_count++;    
        // Check key/value pairs
        if (strcmp(map_it.key, "key1") == 0) {
            cr_assert_str_eq(map_it.value.as_string, "value1", "key1 should map to 'value1'");
            found_key1 = true;
        } else if (strcmp(map_it.key, "key2") == 0) {
            cr_assert_str_eq(map_it.value.as_string, "value2", "key2 should map to 'value2'");
            found_key2 = true;
        } else if (strcmp(map_it.key, "key3") == 0) {
            cr_assert_str_eq(map_it.value.as_string, "value3", "key3 should map to 'value3'");
            found_key3 = true;
        } else {
            cr_assert(false, "Unexpected key: %s", map_it.key);
        }
    }
    
    cr_assert_eq(map_count, 3, "Iterator should yield 3 key-value pairs");
    cr_assert(found_key1 && found_key2 && found_key3, "Iterator should yield all keys");
    
    // Test reset functionality
    cargs_map_reset(&map_it);
    cr_assert_eq(map_it._position, 0, "Reset should set position to 0");
    
    // Verify we can iterate again
    cr_assert(cargs_map_next(&map_it), "Iterator should yield values after reset");
    
    // Clean up
    cargs_free(&cargs);
}
