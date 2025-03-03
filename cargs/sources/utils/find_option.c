#include <string.h>
#include <stdlib.h>
#include "cargs/utils.h"


static cargs_option_t	*__find_from_relative_path(cargs_t *cargs, char *option_name)
{
	for (int i = cargs->subcommand_depth; i >= 0; i--)
	{
		cargs_option_t *options;

		if (i == 0)
			options = cargs->options;
		else
			options = cargs->subcommand_stack[i - 1]->subcommand.options;

		cargs_option_t *option = find_option_by_name(options, option_name);
		if (option != NULL)
			return (option);
	}
	return (NULL);
}


cargs_option_t *find_option_by_active_path(cargs_t *cargs, char *option_path)
{
    cargs_option_t **options = NULL;

    if (option_path == NULL)
        return (NULL);

    char *component = option_path;
    char *delimiter = strchr(option_path, '.');

    if (delimiter == NULL)
        return (__find_from_relative_path(cargs, component));
        
    while (delimiter != NULL)
    {
        
    }

    return (find_option_by_name(options, component));
}
