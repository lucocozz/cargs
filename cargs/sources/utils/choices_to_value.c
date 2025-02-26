#include "cargs/types.h"

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
	return value;
}
