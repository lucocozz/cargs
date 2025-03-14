#include <string.h>
#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/internal/context.h"


cargs_option_t *find_option_by_lname(cargs_option_t *options, const char *lname) 
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        if (options[i].type == TYPE_OPTION && options[i].lname
        && strcmp(options[i].lname, lname) == 0)
            return (&options[i]);
    }
    return (NULL);
}

cargs_option_t *find_option_by_sname(cargs_option_t *options, char sname) 
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        if (options[i].type == TYPE_OPTION && options[i].sname == sname)
            return (&options[i]);
    }
    return (NULL);
}

cargs_option_t *find_positional(cargs_option_t *options, int position) 
{
    int pos_index = 0;

    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        if (options[i].type == TYPE_POSITIONAL) {
            if (pos_index == position)
                return (&options[i]);
            pos_index++;
        }
    }
    return (NULL);
}

cargs_option_t *find_subcommand(cargs_option_t *options, const char *name) 
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        if (options[i].type == TYPE_SUBCOMMAND && strcmp(options[i].name, name) == 0)
            return (&options[i]);
    }
    return (NULL);
}

cargs_option_t *find_option_by_name(cargs_option_t *options, const char *name)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i)
    {
        if (options[i].name && strcmp(options[i].name, name) == 0) {
            return (&options[i]);
        }
    }
    return (NULL);
}

const cargs_option_t *get_active_options(cargs_t *cargs)
{
    const cargs_option_t *command = context_get_subcommand(cargs);
    if (command != NULL)
        return (command->sub_options);
    return (cargs->options);
}

static cargs_option_t	*__find_from_relative_path(cargs_t cargs, const char *option_name)
{
	for (int i = cargs.context.subcommand_depth; i >= 0; --i)
	{
		cargs_option_t *options;

		if (i == 0)
			options = cargs.options;
		else
			options = cargs.context.subcommand_stack[i - 1]->sub_options;

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
		if (component_count > cargs.context.subcommand_depth)
			return (NULL);

	// Format: "subcommand.option_name"
	const char *component = option_path;
	cargs_option_t *options = cargs.options;
	for (size_t i = 0; i < component_count; ++i)
	{
		char *next_dot = strchr(component, '.');
		if (next_dot == NULL)
			break;

		const char *command = cargs.context.subcommand_stack[i]->name;
		size_t component_lenght = next_dot - component;
		if (strncmp(component, command, component_lenght) != 0)
		return (NULL);
		
		component = next_dot + 1;
		options = cargs.context.subcommand_stack[i]->sub_options;
	}

    return (find_option_by_name(options, component));
}
