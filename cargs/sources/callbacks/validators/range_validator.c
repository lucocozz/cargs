#include "cargs/types.h"


cargs_status_t	range_validator(value_t value, validator_data_t data)
{
	if (value.as_int < data.range.min || value.as_int > data.range.max)
		return (cargs_msg_fmt("Value %d is out of range [%d, %d]",
			value.as_int, data.range.min, data.range.max));
	return (cargs_msg_success());
}
