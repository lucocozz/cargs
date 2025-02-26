#include "cargs/types.h"

#include <string.h>


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
