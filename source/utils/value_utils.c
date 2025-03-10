#include "cargs/types.h"

#include <stdlib.h>
#include <string.h>


void	free_option_value(cargs_option_t *option)
{
	if (option->is_allocated == false)
		return;

	if (option->free_handler != NULL)
		option->free_handler(option);
	else
		free(option->value.as_ptr);
}

value_t choices_to_value(value_type_t type, value_t choices, int choices_count, int index)
{
	value_t value = {0};

	if (index < 0 || index >= choices_count)
		return value;

	switch (type) {
		case VALUE_TYPE_INT: value.as_int = choices.as_int_array[index]; break;
		case VALUE_TYPE_STRING: value.as_string = choices.as_string_array[index]; break;
		case VALUE_TYPE_FLOAT: value.as_float = choices.as_float_array[index]; break;
		default: break;
	}
	return (value);
}

int cmp_value(value_type_t type, const value_t a, const value_t b)
{
	if (a.raw == b.raw)
		return 0;

	switch (type) {
		case VALUE_TYPE_INT: return a.as_int - b.as_int;
		case VALUE_TYPE_STRING: return strcmp(a.as_string, b.as_string);
		case VALUE_TYPE_FLOAT: return a.as_float - b.as_float;
		case VALUE_TYPE_BOOL: return a.as_bool - b.as_bool;
		default: return -1;
	}
}
