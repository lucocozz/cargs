// tests/fonctionnel/test_environments.c
#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cargs.h"

// Define options with environment variables
CARGS_OPTIONS(
    test_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('H', "host", "Server hostname",
                ENV_VAR("HOST")),
    OPTION_INT('p', "port", "Server port", 
                FLAGS(FLAG_AUTO_ENV)),
    OPTION_STRING('d', "database", "Database connection string",
                ENV_VAR("DATABASE_URL"),
                FLAGS(FLAG_NO_ENV_PREFIX)),
    OPTION_INT('t', "timeout", "Connection timeout in seconds",
            	DEFAULT(30),
            	ENV_VAR("FORCE_TIMEOUT"),
            	FLAGS(FLAG_ENV_OVERRIDE))
)

// Setup function to set environment variables
void setup_env(void)
{
    setenv("TEST_HOST", "env-server.example.com", 1);
    setenv("TEST_PORT", "9000", 1);
    setenv("DATABASE_URL", "postgres://user:pass@localhost/db", 1);
    setenv("TEST_FORCE_TIMEOUT", "60", 1);
}

// Teardown function to clear environment variables
void teardown_env(void)
{
    unsetenv("TEST_HOST");
    unsetenv("TEST_PORT");
    unsetenv("DATABASE_URL");
    unsetenv("FORCE_TIMEOUT");
}

// Test basic environment variable loading
Test(environments, basic_env, .init = setup_env, .fini = teardown_env)
{
    char *argv[] = {"test"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    cargs.env_prefix = "TEST";
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Environment variables should parse successfully");
    cr_assert_str_eq(cargs_get_value(cargs, "host").as_string, "env-server.example.com", "Host should be from environment");
    cr_assert_eq(cargs_get_value(cargs, "port").as_int, 9000, "Port should be from environment");
    cr_assert_str_eq(cargs_get_value(cargs, "database").as_string, "postgres://user:pass@localhost/db", "Database URL should be from environment");

    cargs_free(&cargs);
}

// Test command line overriding environment variables
Test(environments, cmd_override, .init = setup_env, .fini = teardown_env)
{
    char *argv[] = {"test", "--host=cli-server.example.com", "--port=8888", "--database=mysql://localhost/db"};
    int argc = sizeof(argv) / sizeof(char*);

    cargs_t cargs = cargs_init(test_options, "test", "1.0.0");
    cargs.env_prefix = "TEST";
    int status = cargs_parse(&cargs, argc, argv);
    
    cr_assert_eq(status, CARGS_SUCCESS, "Command line arguments should parse successfully");
    cr_assert_str_eq(cargs_get_value(cargs, "host").as_string, "cli-server.example.com", "Host should be from command line");
    cr_assert_eq(cargs_get_value(cargs, "port").as_int, 8888, "Port should be from command line");
    cr_assert_str_eq(cargs_get_value(cargs, "database").as_string, "mysql://localhost/db", "Database should be from command line");
    
    // Timeout has FLAG_ENV_OVERRIDE, so it should still be from environment
	fprintf(stderr, "timeout: %d\n", cargs_get_value(cargs, "timeout").as_int);
    cr_assert_eq(cargs_get_value(cargs, "timeout").as_int, 60, "Timeout with FLAG_ENV_OVERRIDE should be from environment");
    
    cargs_free(&cargs);
}
