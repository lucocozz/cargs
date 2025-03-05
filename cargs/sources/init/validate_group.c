#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_group_validity(cargs_t *cargs, cargs_option_t *option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);
	
	cargs->active_group = option->name;
	if (option->flags & ~GROUP_FLAG_MASK) {
		status.code = CARGS_ERROR_INVALID_FLAG;
		cargs_push_error(cargs, status);
	}

	return (status);
}
