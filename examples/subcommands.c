/**
 * Subcommands example of cargs library
 * 
 * This example demonstrates how to use cargs to create a CLI tool
 * with subcommands similar to git (git add, git commit, etc.)
 */

#include "cargs.h"
#include <stdio.h>


int add_command(cargs_t *cargs, void *data)
{
	(void)data;

	const char *file = cargs_get_value(*cargs, "add.file").as_string;
	bool force = cargs_get_value(*cargs, "add.force").as_bool;
	bool verbose = cargs_get_value(*cargs, "verbose").as_bool;

	printf("Adding file: %s\n", file);
	if (force) printf("  with force option\n");
	if (verbose) printf("  with verbose output\n");

	return 0;
}

int remove_command(cargs_t *cargs, void *data)
{
	(void)data;

	const char* file = cargs_get_value(*cargs, "file").as_string;
	bool recursive = cargs_get_value(*cargs, "rm.recursive").as_bool;

	printf("Removing file: %s\n", file);
	if (recursive) printf("  recursively\n");

	return 0;
}


// Define options for "add" subcommand
CARGS_OPTIONS(
	add_options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('f', "force", "Force add"),
	OPTION_FLAG('v', "verbose", "Verbose output"),
	POSITIONAL_STRING("file", "File to add", FLAGS(FLAG_REQUIRED))
)

// Define options for "remove" subcommand
CARGS_OPTIONS(
	remove_options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('r', "recursive", "Recursive remove"),
	POSITIONAL_STRING("file", "File to remove", CHOICES_STRING("file1", "file2"))
)

// Define main options with subcommands
CARGS_OPTIONS(
	options,
	HELP_OPTION(FLAGS(FLAG_EXIT)),
	VERSION_OPTION(FLAGS(FLAG_EXIT)),
	OPTION_FLAG('d', "debug", "Enable debug mode"),

	// Define subcommands
	SUBCOMMAND("add", add_options, HELP("Add files to the index"), ACTION(add_command)),
	SUBCOMMAND("rm", remove_options, HELP("Remove files from the index"), ACTION(remove_command), FLAGS(FLAG_DEPRECATED))
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
	if (debug) printf("Debug mode enabled\n");
	 
	// Check which subcommand was used
	if (cargs_have_subcommand(cargs)) {
		// Execute the subcommand
		status = cargs_execute_command(&cargs, NULL);
		if (status != CARGS_SUCCESS)
			return status;
	}
	else
		printf("No command specified. Use --help to see available commands.\n");

	// Free resources
	cargs_free(&cargs);
	return 0;
}
