#include "cargs.h"
#include "cargs/api/debug.h"

#include <stdio.h>

// Exemple d'utilisation
int custom_handler(cargs_t *cargs, cargs_option_t *options, char *arg) {
    printf("Handler called with: %s\n", arg);
    return (0);
}

CARGS_OPTIONS(
    toto,
    OPTION_INT('b', "beta", "Beta value"),
    OPTION_INT('a', "alpha", "Alpha value",
                CONFLICTS("beta"),
                HANDLER(custom_handler)),
);


CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(),

    GROUP_START("Network", FLAGS(FLAG_EXCLUSIVE)),
    OPTION_INT('p', "port", "Port number",
                DEFAULT(8080),
                CHOICES_INT(80, 443, 8080)),
                // REQUIRES("host")),
    OPTION_STRING('H', "host", "Host name",
                // DEFAULT("localhost"),
                // CONFLICTS("port"),
                CHOICES_STRING("localhost", "127.0.0.1", "::1")),
    GROUP_END(),

    SUBCOMMAND("toto", toto),
    OPTION_INT('t', "timeout", "Timeout in seconds",
                DEFAULT(30),
                RANGE(1, 60),
                HANDLER(custom_handler)),
    POSITIONAL_STRING("file", "File to process"),
);

int main(int argc, char **argv)
{
    // print_options(options);
	cargs_t cargs = cargs_init(options, "test", "1.0.0", "Test program");
	cargs_parse(&cargs, argc, argv);
	return (0);
}
