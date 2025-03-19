// tests/unit/test_utils/test_value_utils.c
#include <criterion/criterion.h>
#include "cargs/internal/utils.h"

Test(value_utils, compare_values)
{
    // Test comparing integers
    value_t a = {.as_int = 42};
    value_t b = {.as_int = 42};
    value_t c = {.as_int = 100};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_INT, a, b), 0, "Equal integers should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_INT, a, c), 0, "42 should be less than 100");
    cr_assert_gt(cmp_value(VALUE_TYPE_INT, c, a), 0, "100 should be greater than 42");
    
    // Test comparing strings
    value_t str1 = {.as_string = "abc"};
    value_t str2 = {.as_string = "abc"};
    value_t str3 = {.as_string = "def"};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_STRING, str1, str2), 0, "Equal strings should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_STRING, str1, str3), 0, "abc should be less than def");
    cr_assert_gt(cmp_value(VALUE_TYPE_STRING, str3, str1), 0, "def should be greater than abc");
    
    // Test comparing booleans
    value_t true_val = {.as_bool = true};
    value_t false_val = {.as_bool = false};
    
    cr_assert_eq(cmp_value(VALUE_TYPE_BOOL, true_val, true_val), 0, "Equal booleans should compare as equal");
    cr_assert_lt(cmp_value(VALUE_TYPE_BOOL, false_val, true_val), 0, "false should be less than true");
    cr_assert_gt(cmp_value(VALUE_TYPE_BOOL, true_val, false_val), 0, "true should be greater than false");
}

Test(value_utils, choices_to_value)
{
    // Test integer choices
    value_t int_choices = {.as_array_int = (int64_t[]){10, 20, 30, 40, 50}};
    
    value_t result1 = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 2);
    cr_assert_eq(result1.as_int, 30, "Third element should be 30");
    
    value_t result2 = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 0);
    cr_assert_eq(result2.as_int, 10, "First element should be 10");
    
    // Test with invalid index
    value_t invalid = choices_to_value(VALUE_TYPE_INT, int_choices, 5, 10);
    cr_assert_eq(invalid.raw, 0, "Invalid index should return empty value");
    
    // Test string choices
    value_t str_choices = {.as_array_string = (char*[]){"one", "two", "three"}};
    
    value_t str_result = choices_to_value(VALUE_TYPE_STRING, str_choices, 3, 1);
    cr_assert_str_eq(str_result.as_string, "two", "Second element should be 'two'");
}

Test(value_utils, free_option_value)
{
    // This test primarily checks that free_option_value doesn't crash
    // Not a thorough test but ensures basic functionality
    
    cargs_option_t option = {
        .is_allocated = false,
        .value = {.as_int = 42}
    };
    
    // Should not try to free since is_allocated is false
    free_option_value(&option);
    cr_assert(true, "free_option_value should not crash with non-allocated values");
    
    // Test with a real allocation would require more setup and mocking
}
