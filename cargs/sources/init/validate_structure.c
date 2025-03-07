#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


cargs_error_t ensure_subcommand_validity(cargs_t *cargs, cargs_option_t *option);
cargs_error_t ensure_group_validity(cargs_t *cargs, cargs_option_t *option);
cargs_error_t ensure_option_validity(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option);
cargs_error_t ensure_positional_validity(cargs_t *cargs, cargs_option_t *option);


static cargs_error_t __ensure_validity(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
	switch (option->type) {
		case TYPE_OPTION: return ensure_option_validity(cargs, options, option);
		case TYPE_POSITIONAL: return ensure_positional_validity(cargs, option);
		case TYPE_GROUP: return ensure_group_validity(cargs, option);
		case TYPE_SUBCOMMAND: return ensure_subcommand_validity(cargs, option);
		default: return CARGS_ERROR(
			CARGS_ERROR_MALFORMED_OPTION, "Invalid option type",
			CARGS_ERROR_CONTEXT(cargs, option)
		);
	}
}

static cargs_error_t	__is_unique(cargs_t *cargs, cargs_option_t *option, cargs_option_t *other_option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);


	if (option->name && other_option->name
	&& strcmp(option->name, other_option->name) == 0) {
		status.code = CARGS_ERROR_DUPLICATE_OPTION;
		status.message = "Name must be unique";
	}

	if (option->sname && other_option->sname
	&& option->sname == other_option->sname) {
		status.code = CARGS_ERROR_DUPLICATE_OPTION;
		status.message = "Short name must be unique";
	}

	if (option->lname && other_option->lname
	&& strcmp(option->lname, other_option->lname) == 0) {
		status.code = CARGS_ERROR_DUPLICATE_OPTION;
		status.message = "Long name must be unique";
	}

	return (status);
}

int validate_structure(cargs_t *cargs, cargs_option_t *options)
{
	cargs_error_t status;

	for (int i = 0; options[i].type != TYPE_NONE; ++i)
	{
		cargs_option_t *option = &options[i];
		context_set_option(cargs, option);

		status = __ensure_validity(cargs, options, option);

		for (int j = i + 1; options[j].type != TYPE_NONE; ++j)
		{
			cargs_option_t *other_option = &options[j];

			if (option->type != other_option->type)
				continue;
			status = __is_unique(cargs, option, other_option);
			if (status.code != CARGS_SUCCESS)
				cargs_push_error(cargs, status);
		}

		// Validate subcommand options recursively
		if (option->type == TYPE_SUBCOMMAND && option->subcommand.options != NULL)
		{
			context_push_subcommand(cargs, option);
			validate_structure(cargs, option->subcommand.options);
			context_pop_subcommand(cargs);
		}
	}
	return (status.code);
}
