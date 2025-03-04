#include "cargs.h"
#include "cargs/api/debug.h"

#include <stdio.h>


int custom_handler(cargs_t *cargs, cargs_option_t *options, char *arg) {
    printf("Handler called with: %s\n", arg);
    return (0);
}

CARGS_OPTIONS(
    toto,
    OPTION_INT('b', "beta", "Beta value", DEFAULT(21)),
    OPTION_INT('a', "alpha", "Alpha value",
                CONFLICTS("beta")),
);

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(),

    GROUP_START("Network", FLAGS(FLAG_EXCLUSIVE)),
    OPTION_INT('p', "port", "Port number",
                DEFAULT(8080),
                CHOICES_INT(80, 443, 8080)),
    OPTION_STRING('H', "host", "Host name",
                CHOICES_STRING("localhost", "127.0.0.1", "::1")),
    GROUP_END(),

    SUBCOMMAND("toto", toto),
    OPTION_INT('t', "timeout", "Timeout in seconds",
                DEFAULT(30),
                RANGE(1, 60),
                HANDLER(custom_handler)),
    OPTION_INT('b', "beta", "Beta value", DEFAULT(42)),
    POSITIONAL_STRING("file", "File to process"),
);

int main(int argc, char **argv)
{
	cargs_t cargs = cargs_init(options, "Test", "1.0.0", "Test program");
    cargs_parse(&cargs, argc, argv);

    printf("beta: %d\n", cargs_get_value(cargs, "beta").as_int);
    printf(".beta: %d\n", cargs_get_value(cargs, ".beta").as_int);
    printf("toto.beta: %d\n", cargs_get_value(cargs, "toto.beta").as_int);
	return (0);
}
