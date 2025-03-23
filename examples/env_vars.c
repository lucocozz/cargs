/**
 * Environment variables example for cargs
 * 
 * This example demonstrates different ways to use environment variables:
 * - Explicit environment variable names
 * - Auto-generated environment variable names
 * - Environment variables that override command line arguments
 * - Control over environment variable name prefixing
 * 
 * Try running with different environment variables:
 * 
 *   # Set environment variables
 *   export APP_HOST=env-server.example.com
 *   export APP_PORT=9000
 *   export DATABASE_URL=postgres://user:pass@localhost/db
 *   export DEBUG=1
 *   export FORCE_TIMEOUT=60
 * 
 *   # Run with no arguments (values from environment)
 *   ./env_variables
 * 
 *   # Run with command line arguments (override environment except for FORCE_TIMEOUT)
 *   ./env_variables --host=cli-server.example.com --port=8080 --timeout=30
 */

#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

CARGS_OPTIONS(
	options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	VERSION_OPTION(FLAGS(FLAG_EXIT)),

	// Option 1: Explicit environment variable with prefix
	// Will look for APP_HOST environment variable
	OPTION_STRING('H', "host", "Server hostname",
				DEFAULT("localhost"),
				ENV_VAR("HOST")),
	
	// Option 2: Auto-generated environment variable name
	// Will generate APP_PORT from the option name
	OPTION_INT('p', "port", "Server port", 
			   DEFAULT(8080),
			   FLAGS(FLAG_AUTO_ENV)),
	
	// Option 3: Explicit environment variable without prefix
	// Will look for DATABASE_URL exactly as specified
	OPTION_STRING('d', "database", "Database connection string",
				ENV_VAR("DATABASE_URL"),
				FLAGS(FLAG_NO_ENV_PREFIX)),
	
	// Option 4: Auto-generated name without prefix
	// Will generate VERBOSE from the option name
	OPTION_FLAG('v', "verbose", "Enable verbose output",
				FLAGS(FLAG_AUTO_ENV | FLAG_NO_ENV_PREFIX)),
	
	// Option 5: Environment variable that can override command line
	// Even if --timeout is specified, FORCE_TIMEOUT will take precedence
	OPTION_INT('t', "timeout", "Connection timeout in seconds",
			   DEFAULT(30),
			   ENV_VAR("FORCE_TIMEOUT"),
			   FLAGS(FLAG_ENV_OVERRIDE)),
			  
	// Option 6: Debug flag with default prefix
	OPTION_FLAG('\0', "debug", "Enable debug mode",
			   ENV_VAR("DEBUG"))
)

int main(int argc, char **argv)
{
	// Initialize cargs and set environment prefix
	// This prefix will be applied to all environment variables 
	// unless FLAG_NO_ENV_PREFIX is specified
	cargs_t cargs = cargs_init(options, "env_variables", "1.0.0");
	cargs.description = "Example of environment variables usage";
	cargs.env_prefix = "APP";
	
	int status = cargs_parse(&cargs, argc, argv);
	if (status != CARGS_SUCCESS)
		return status;

	// Access option values as usual
	const char* host = cargs_get(cargs, "host").as_string;
	int port = cargs_get(cargs, "port").as_int;
	const char* database = cargs_get(cargs, "database").as_string;
	bool verbose = cargs_get(cargs, "verbose").as_bool;
	int timeout = cargs_get(cargs, "timeout").as_int;
	bool debug = cargs_get(cargs, "debug").as_bool;
	
	// Display configuration
	printf("═════════════════════════════════════════\n");
	printf("    ENVIRONMENT VARIABLES CONFIGURATION   \n");
	printf("═════════════════════════════════════════\n\n");
	
	printf("Server Configuration:\n");
	printf("  Host:      %s\n", host);
	printf("  Port:      %d\n", port);
	printf("  Database:  %s\n", database && *database ? database : "(not set)");
	printf("  Timeout:   %d seconds\n", timeout);
	
	printf("\nDebug Settings:\n");
	printf("  Verbose:   %s\n", verbose ? "enabled" : "disabled");
	printf("  Debug:     %s\n", debug ? "enabled" : "disabled");
	
	printf("\nEnvironment Variables Used:\n");
	printf("  APP_HOST:       Explicit with prefix\n");
	printf("  APP_PORT:       Auto-generated with prefix\n");
	printf("  DATABASE_URL:   Explicit without prefix\n");
	printf("  VERBOSE:        Auto-generated without prefix\n"); 
	printf("  FORCE_TIMEOUT:  Overrides command line\n");
	printf("  APP_DEBUG:      Explicit with prefix\n");
	
	printf("\nCurrent Environment Variable Values:\n");
	printf("  APP_HOST:       %s\n", getenv("APP_HOST") ? getenv("APP_HOST") : "(not set)");
	printf("  APP_PORT:       %s\n", getenv("APP_PORT") ? getenv("APP_PORT") : "(not set)");
	printf("  DATABASE_URL:   %s\n", getenv("DATABASE_URL") ? getenv("DATABASE_URL") : "(not set)");
	printf("  VERBOSE:        %s\n", getenv("VERBOSE") ? getenv("VERBOSE") : "(not set)");
	printf("  FORCE_TIMEOUT:  %s\n", getenv("FORCE_TIMEOUT") ? getenv("FORCE_TIMEOUT") : "(not set)");
	printf("  APP_DEBUG:      %s\n", getenv("APP_DEBUG") ? getenv("APP_DEBUG") : "(not set)");
	
	cargs_free(&cargs);
	return 0;
}
