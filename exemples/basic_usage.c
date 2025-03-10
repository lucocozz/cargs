#include "cargs.h"
#include "cargs/api/debug.h"

#include <stdio.h>


int custom_handler(cargs_t *cargs, cargs_option_t *options, char *arg) {
    printf("Handler called with: %s\n", arg);
    return (0);
}

CARGS_OPTIONS(
    toto,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_INT('b', "beta", "Beta value", DEFAULT(21)),
    OPTION_INT('a', "alpha", "Alpha value"),
);

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(),

    GROUP_START("Network", FLAGS(FLAG_EXCLUSIVE)),
    OPTION_INT('p', "port", "Port number"),
    OPTION_STRING('H', "host", "Host name"),
    GROUP_END(),

    SUBCOMMAND("toto", toto),
    OPTION_INT('t', "timeout", "Timeout in seconds"),
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

    cargs_free(&cargs);
	return (0);
}
