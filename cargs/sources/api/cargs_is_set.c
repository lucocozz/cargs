#include "cargs/utils.h"


bool	cargs_is_set(cargs_t cargs, const char *option_path)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	if (option == NULL)
		return (false);
	return (option->is_set);
}
