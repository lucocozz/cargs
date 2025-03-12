/**
 * Subcommands example of cargs library
 * 
 * This example demonstrates how to use cargs to create a CLI tool
 * with subcommands similar to git (git add, git commit, etc.)
 */

#include "cargs.h"
#include <stdio.h>

// Define options for "add" subcommand
CARGS_OPTIONS(
	add_options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('f', "force", "Force add"),
	OPTION_FLAG('v', "verbose", "Verbose output"),
	POSITIONAL_STRING("file", "File to add")
)

// Define options for "remove" subcommand
CARGS_OPTIONS(
	remove_options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('r', "recursive", "Recursive remove"),
	POSITIONAL_STRING("file", "File to remove")
)

// Define main options with subcommands
CARGS_OPTIONS(
	options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	VERSION_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('d', "debug", "Enable debug mode"),

	// Define subcommands
	SUBCOMMAND("add", add_options),
	SUBCOMMAND("remove", remove_options)
)

int main(int argc, char **argv)
{
	// Initialize cargs
	cargs_t cargs = cargs_init(options, "subcommands_example", "1.0.0", 
								"Example of cargs library with subcommands");

	// Parse command-line arguments
	int status = cargs_parse(&cargs, argc, argv);
	if (status != CARGS_SUCCESS)
		return status;

	// Check global options
	bool debug = cargs_get_value(cargs, "debug").as_bool;
	if (debug)
		printf("Debug mode enabled\n");
	 
	 // Check which subcommand was used
	if (cargs_is_set(cargs, "add")) {
		const char* file = cargs_get_value(cargs, "add.file").as_string;
		bool force = cargs_get_value(cargs, "add.force").as_bool;
		bool verbose = cargs_get_value(cargs, "add.verbose").as_bool;

		printf("Adding file: %s\n", file);
		if (force) printf("  with force option\n");
		if (verbose) printf("  with verbose output\n");
	}
	else if (cargs_is_set(cargs, "remove")) {
		const char* file = cargs_get_value(cargs, "remove.file").as_string;
		bool recursive = cargs_get_value(cargs, "remove.recursive").as_bool;

		printf("Removing file: %s\n", file);
		if (recursive) printf("  recursively\n");
	}
	else {
		printf("No subcommand specified. Use --help to see available commands.\n");
	}
	 
	// Free resources
	cargs_free(&cargs);
	return 0;
}
