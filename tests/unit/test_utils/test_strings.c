#include <criterion/criterion.h>
#include "cargs/internal/utils.h"


Test(strings, starts_with_valid)
{
    // Test basic prefix matching
    cr_assert_not_null(starts_with("pre", "prefix"));
    
    // Test the returned pointer position
    char *result = starts_with("pre", "prefix");
    cr_assert_eq(result, "prefix" + 3);
    cr_assert_str_eq(result, "fix");
    
    // Test with empty prefix
    cr_assert_not_null(starts_with("", "text"));
    cr_assert_eq(starts_with("", "text"), "text");
}

Test(strings, starts_with_invalid)
{
    // Test with non-matching prefix
    cr_assert_null(starts_with("abc", "def"));
    
    // Test with NULL inputs
    cr_assert_null(starts_with(NULL, "text"));
    cr_assert_null(starts_with("pre", NULL));
    cr_assert_null(starts_with(NULL, NULL));
    
    // Test with prefix longer than string
    cr_assert_null(starts_with("long_prefix", "short"));
}
