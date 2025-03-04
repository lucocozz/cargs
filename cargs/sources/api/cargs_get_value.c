#include "cargs/utils.h"
#include "cargs/types.h"


value_t	cargs_get_value(cargs_t cargs, const char *option_path)
{
	cargs_option_t *option = find_option_by_active_path(cargs, option_path);
	if (option == NULL)
		return ((value_t){.raw = 0});
	return (option->value);
}
