#include "cargs/types.h"

#include <assert.h>

bool	range_validator(value_t value, validator_data_t data)
{
	if (value.as_int < data.range.min || value.as_int > data.range.max)
		return (false);
	return (true);
}
