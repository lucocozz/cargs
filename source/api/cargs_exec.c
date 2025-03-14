#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/utils.h"
#include "cargs/internal/context.h"


bool cargs_have_subcommand(cargs_t cargs) {
	return (cargs.context.subcommand_depth > 0);
}

int	cargs_execute_command(cargs_t *cargs, void *data)
{
	const cargs_option_t *command = context_get_subcommand(cargs);
	if (command == NULL) {
		CARGS_REPORT_ERROR(cargs, CARGS_ERROR_NO_COMMAND,
			"Internal error: No command to execute");
	}
	if (command->action == NULL) {
		CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_HANDLER,
			"Internal error: Command %s has no action handler", command->name);
	}
	return (command->action(cargs, data));
}
