// tests/fonctionnel/test_multi_values.c
#include <criterion/criterion.h>
#include "cargs.h"

// Define options with arrays and maps
CARGS_OPTIONS(
    test_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_ARRAY_STRING('s', "strings", "Array of strings", 
                       FLAGS(FLAG_SORTED)),
    OPTION_ARRAY_INT('i', "ints", "Array of integers", 
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    OPTION_MAP_STRING('e', "env", "Environment variables"),
    OPTION_MAP_INT('p', "ports", "Service ports")
)

// Test array of strings
Test(multi_values, array_string)
{
    char *argv[] = {"test", "--strings=one,two,three"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Array of strings should parse successfully");
    cr_assert_eq(cargs_count(cargs, "strings"), 3, "Should have 3 values");
    
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 0).as_string, "one");
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 1).as_string, "three");
    cr_assert_str_eq(cargs_array_get(cargs, "strings", 2).as_string, "two");
    
    cargs_free(&cargs);
}

// Test array of integers
Test(multi_values, array_int)
{
    char *argv[] = {"test", "--ints=5,3,1,3,5"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Array of integers should parse successfully");
    cr_assert_eq(cargs_count(cargs, "ints"), 3, "Should have 3 unique values");
    
    cr_assert_eq(cargs_array_get(cargs, "ints", 0).as_int, 1);
    cr_assert_eq(cargs_array_get(cargs, "ints", 1).as_int, 3);
    cr_assert_eq(cargs_array_get(cargs, "ints", 2).as_int, 5);
    
    cargs_free(&cargs);
}

// Test array range notation
Test(multi_values, array_int_range)
{
    char *argv[] = {"test", "--ints=1-5"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Array range should parse successfully");
    cr_assert_eq(cargs_count(cargs, "ints"), 5, "Should expand to 5 values");
    
    for (int i = 0; i < 5; i++) {
        cr_assert_eq(cargs_array_get(cargs, "ints", i).as_int, i + 1);
    }
    
    cargs_free(&cargs);
}

// Test map of strings
Test(multi_values, map_string)
{
    char *argv[] = {"test", "--env=HOME=/home/user,USER=test,SHELL=/bin/bash"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Map of strings should parse successfully");
    cr_assert_eq(cargs_count(cargs, "env"), 3, "Should have 3 key-value pairs");
    
    cr_assert_str_eq(cargs_map_get(cargs, "env", "HOME").as_string, "/home/user");
    cr_assert_str_eq(cargs_map_get(cargs, "env", "USER").as_string, "test");
    cr_assert_str_eq(cargs_map_get(cargs, "env", "SHELL").as_string, "/bin/bash");
    
    cargs_free(&cargs);
}

// Test iterators
Test(multi_values, iterators)
{
    char *argv[] = {"test", "--ints=1,2,3", "--env=a=1,b=2"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Options should parse successfully");
    
    // Test array iterator
    cargs_array_iterator_t it = cargs_array_iterator(cargs, "ints");
    cr_assert_eq(it.count, 3, "Iterator should have 3 elements");
    
    value_t val;
    int count = 0;
    while (cargs_array_next(&it, &val)) {
        count++;
        cr_assert_eq(val.as_int, count, "Iterator values should match");
    }
    cr_assert_eq(count, 3, "Iterator should have yielded 3 values");
    
    // Test map iterator
    cargs_map_iterator_t map_it = cargs_map_iterator(cargs, "env");
    cr_assert_eq(map_it.count, 2, "Map iterator should have 2 elements");
    
    const char *key;
    count = 0;
    while (cargs_map_next(&map_it, &key, &val))
	{
        count++;
        if (strcmp(key, "a") == 0)
            cr_assert_eq(val.as_int, 1, "Key 'a' should map to 1");
        else if (strcmp(key, "b") == 0)
            cr_assert_eq(val.as_int, 2, "Key 'b' should map to 2");
		else {
			cr_assert_fail("Unexpected key");
			fprintf(stderr, "Unexpected key: %s\n", key);
		}
    }
    cr_assert_eq(count, 2, "Map iterator should have yielded 2 key-value pairs");
    
    cargs_free(&cargs);
}
