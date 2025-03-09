#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_group_validity(cargs_t *cargs, cargs_option_t *option)
{
	context_set_group(cargs, option);
	if (option->flags & ~GROUP_FLAG_MASK) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_GROUP,
			"Invalid flags for group '%s'", option->name);
	}

	return (CARGS_OK());
}
