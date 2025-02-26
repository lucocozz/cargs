#include "cargs.h"
#include "cargs/debug.h"

#include <stdio.h>

// Exemple d'utilisation
void custom_handler(cargs_option_t *options, char *arg) {
    printf("Handler called with: %s\n", arg);
}

CARGS_OPTIONS(
    toto,
    OPTION_INT('a', "alpha", "Alpha value",
                REQUIRES("gamma"),
                CONFLICTS("beta", "gamma"),
                HANDLER(custom_handler)),
    OPTION_INT('b', "beta", "Beta value"),
);


CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),

    OPTION_INT('p', "port", "Port number",
        DEFAULT(8080),
        CHOICES_INT(80, 443, 8080),
        REQUIRES("host", "protocol")),
    GROUP_START("Network"),
    OPTION_STRING('H', "host", "Host name",
                DEFAULT("localhos"),
                HANDLER(custom_handler),
                REQUIRES("port", "protocol"),
                CHOICES_STRING("localhost", "127.0.0.1", "::1"),
                CONFLICTS("socket")),
    GROUP_END(),

    SUBCOMMAND("toto", toto),
    OPTION_INT('H', "timeout", "Timeout in seconds",
                DEFAULT(30),
                RANGE(1, 60),
                HANDLER(custom_handler)),
);

int main(int argc, char **argv)
{
    // print_options(options);
	cargs_t cargs = cargs_init(options, "test", "1.0.0", NULL);
	// cargs_parse(cargs, argc, argv);
	return (0);
}
