#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_subcommand_validity(cargs_t *cargs, cargs_option_t *option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);

	if (option->name == NULL) {
		status.code = CARGS_ERROR_MALFORMED_OPTION;
		status.message = "Subcommand must have a name";
		cargs_push_error(cargs, status);
	}

	if (option->subcommand.options == NULL) {
		status.code = CARGS_ERROR_MALFORMED_OPTION;
		status.message = "Subcommand must have options";
		cargs_push_error(cargs, status);
	}

	if (option->flags & ~SUBCOMMAND_FLAG_MASK) {
		status.code = CARGS_ERROR_INVALID_FLAG;
		cargs_push_error(cargs, status);
	}

	return (status);
}
