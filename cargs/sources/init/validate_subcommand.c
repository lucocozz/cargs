#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_subcommand_validity(cargs_t *cargs, cargs_option_t *option)
{
	if (option->name == NULL) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
			"Subcommand must have a name");
	}

	if (option->subcommand.options == NULL) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION,
			"Subcommand '%s' must have options", option->name);
	}

	if (option->flags & ~SUBCOMMAND_FLAG_MASK) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_INVALID_FLAG,
			"Invalid flags for subcommand '%s'", option->name);
	}

	return (CARGS_OK());
}
