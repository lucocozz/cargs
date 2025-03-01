#include <string.h>
#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


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
