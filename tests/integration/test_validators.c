// tests/integration/test_validators.c
#include <criterion/criterion.h>
#include "cargs.h"
#include "cargs/regex.h"

// Define test options with validators
CARGS_OPTIONS(
    test_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_INT('p', "port", HELP("Port number"), 
                DEFAULT(8080), RANGE(1, 65535)),
    OPTION_STRING('l', "level", HELP("Log level"), 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    OPTION_STRING('e', "email", HELP("Email address"), 
                REGEX(MAKE_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", 
                    "Enter email format"))),
    // Add LENGTH validator test option
    OPTION_STRING('u', "username", HELP("Username"), 
                LENGTH(3, 16)),
    // Add COUNT validator test option
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags"), 
                COUNT(2, 5))
)

// Test for range validation
Test(validators_integration, range_validation_success)
{
    // Valid port number
    char *argv[] = {"test", "-p", "8000"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid port should pass validation");
    cr_assert_eq(cargs_get(cargs, "port").as_int, 8000);
    
    cargs_free(&cargs);
}

Test(validators_integration, range_validation_failure)
{
    // Invalid port number (out of range)
    char *argv[] = {"test", "-p", "90000"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Out of range port should fail validation");
    
    cargs_free(&cargs);
}

// Test for choices validation
Test(validators_integration, choices_validation_success)
{
    // Valid log level
    char *argv[] = {"test", "-l", "warning"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid log level should pass validation");
    cr_assert_str_eq(cargs_get(cargs, "level").as_string, "warning");
    
    cargs_free(&cargs);
}

Test(validators_integration, choices_validation_failure)
{
    // Invalid log level
    char *argv[] = {"test", "-l", "critical"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Invalid log level should fail validation");
    
    cargs_free(&cargs);
}

// Test for regex validation
Test(validators_integration, regex_validation_success)
{
    // Valid email
    char *argv[] = {"test", "-e", "test@example.com"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid email should pass validation");
    cr_assert_str_eq(cargs_get(cargs, "email").as_string, "test@example.com");
    
    cargs_free(&cargs);
}

Test(validators_integration, regex_validation_failure)
{
    // Invalid email
    char *argv[] = {"test", "-e", "invalid-email"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Invalid email should fail validation");
    
    cargs_free(&cargs);
}

// Test for length validation
Test(validators_integration, length_validation_success)
{
    // Valid username length
    char *argv[] = {"test", "-u", "johndoe"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid username length should pass validation");
    cr_assert_str_eq(cargs_get(cargs, "username").as_string, "johndoe");
    
    cargs_free(&cargs);
}

Test(validators_integration, length_validation_failure_too_short)
{
    // Username too short
    char *argv[] = {"test", "-u", "jo"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Too short username should fail validation");
    
    cargs_free(&cargs);
}

Test(validators_integration, length_validation_failure_too_long)
{
    // Username too long
    char *argv[] = {"test", "-u", "johndoethisiswaytoolong"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Too long username should fail validation");
    
    cargs_free(&cargs);
}

// Test for count validation
Test(validators_integration, count_validation_success)
{
    // Valid number of tags
    char *argv[] = {"test", "-t", "tag1", "-t", "tag2", "-t", "tag3"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Valid number of tags should pass validation");
    cr_assert_eq(cargs_count(cargs, "tags"), 3, "Should have 3 tags");
    
    cargs_free(&cargs);
}

Test(validators_integration, count_validation_failure_too_few)
{
    // No tags (too few)
    char *argv[] = {"test", "-t", "tag1"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Too few tags should fail validation");
    
    cargs_free(&cargs);
}

Test(validators_integration, count_validation_failure_too_many)
{
    // Too many tags
    char *argv[] = {"test", "-t", "tag1", "-t", "tag2", "-t", "tag3", 
                    "-t", "tag4", "-t", "tag5", "-t", "tag6"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_neq(status, CARGS_SUCCESS, "Too many tags should fail validation");
    
    cargs_free(&cargs);
}
