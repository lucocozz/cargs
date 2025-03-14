#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/internal/context.h"
#include "cargs/internal/utils.h"


cargs_error_t validate_subcommand(cargs_t *cargs, cargs_option_t *option);
cargs_error_t validate_group(cargs_t *cargs, cargs_option_t *option);
cargs_error_t validate_option(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option);
cargs_error_t validate_positional(cargs_t *cargs, cargs_option_t *option);


static cargs_error_t __ensure_validity(cargs_t *cargs, cargs_option_t *options, cargs_option_t *option)
{
	switch (option->type) {
		case TYPE_OPTION: return validate_option(cargs, options, option);
		case TYPE_POSITIONAL: return validate_positional(cargs, option);
		case TYPE_GROUP: return validate_group(cargs, option);
		case TYPE_SUBCOMMAND: return validate_subcommand(cargs, option);
		default:
			CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MALFORMED_OPTION, "Invalid option type");
			return (cargs_error_t){ .code = CARGS_ERROR_MALFORMED_OPTION };
			
	}
}

static cargs_error_t	__is_unique(cargs_t *cargs, cargs_option_t *option, cargs_option_t *other_option)
{
	if (option->name && other_option->name
	&& strcmp(option->name, other_option->name) == 0) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION,
			"%s: Name must be unique", option->name);
	}

	if (option->sname && other_option->sname
	&& option->sname == other_option->sname) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION,
			"%c: Short name must be unique", option->sname);
	}

	if (option->lname && other_option->lname
	&& strcmp(option->lname, other_option->lname) == 0) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_DUPLICATE_OPTION,
			"%s: Long name must be unique", option->lname);
	}

	return (CARGS_OK());
}

cargs_error_t validate_structure(cargs_t *cargs, cargs_option_t *options)
{
	bool have_helper = false;
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
		}

		if (option->type == TYPE_OPTION && strcmp(option->name, "help") == 0)
			have_helper = true;

		// Validate sub_options recursively
		if (option->type == TYPE_SUBCOMMAND && option->sub_options != NULL)
		{
			context_push_subcommand(cargs, option);
			validate_structure(cargs, option->sub_options);
			context_pop_subcommand(cargs);
		}
	}
	if (!have_helper) {
		CARGS_COLLECT_ERROR(cargs, CARGS_ERROR_MISSING_HELP,
			"Missing 'help' option");
	}
	return (status);
}
