#include <string.h>
#include <stdlib.h>
#include "cargs/utils.h"

/*
** option path format:
** 1 "relative": "option_name" -> boucler sur tous les active subcommands jusqu'à trouver l'option
** 2 "absolute": "subcommand.option_name" -> vérifier que le path correspond aux subcommands actives
*/


static cargs_option_t	*__find_from_relative_path(cargs_t cargs, const char *option_name)
{
	for (int i = cargs.subcommand_depth; i >= 0; --i)
	{
		cargs_option_t *options;

		if (i == 0)
			options = cargs.options;
		else
			options = cargs.subcommand_stack[i - 1]->subcommand.options;

		cargs_option_t *option = find_option_by_name(options, option_name);
		if (option != NULL)
			return (option);
	}
	return (NULL);
}

static size_t	__count_components(const char *path)
{
	size_t count = 0;

	for (const char *c = path; *c; ++c)
		if (*c == '.')
			count++;
	return (count);
}

cargs_option_t *find_option_by_active_path(cargs_t cargs, const char *option_path)
{
    if (option_path == NULL)
        return (NULL);

	// Format: "option_name"
    if (strchr(option_path, '.') == NULL)
		return (__find_from_relative_path(cargs, option_path));

	// Format: ".option_name" (root)
	if (option_path[0] == '.')
		return (find_option_by_name(cargs.options, option_path + 1));

	size_t component_count = __count_components(option_path);
		if (component_count > cargs.subcommand_depth)
			return (NULL);

	// Format: "subcommand.option_name"
	const char *component = option_path;
	cargs_option_t *options = cargs.options;
	for (size_t i = 0; i < component_count; ++i)
	{
		char *next_dot = strchr(component, '.');
		if (next_dot == NULL)
			break;

		const char *command = cargs.subcommand_stack[i]->name;
		size_t component_lenght = next_dot - component;
		if (strncmp(component, command, component_lenght) != 0)
		return (NULL);
		
		component = next_dot + 1;
		options = cargs.subcommand_stack[i]->subcommand.options;
	}

    return (find_option_by_name(options, component));
}
