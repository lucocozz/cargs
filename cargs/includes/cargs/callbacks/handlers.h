#ifndef CARGS_CALLBACKS_HANDLERS_H
#define CARGS_CALLBACKS_HANDLERS_H


#include "cargs/types.h"


int	string_handler(cargs_option_t *options, char *arg);
int	int_handler(cargs_option_t *options, char *arg);
int	bool_handler(cargs_option_t *options, char *arg);
int	float_handler(cargs_option_t *options, char *arg);

int	help_handler(cargs_option_t *options, char *arg);
int	version_handler(cargs_option_t *options, char *arg);


#endif // CARGS_CALLBACKS_HANDLERS_H
