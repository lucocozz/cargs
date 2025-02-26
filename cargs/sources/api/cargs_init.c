#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/utils.h"


static cargs_error_t __ensure_option_validity(cargs_t *cargs, cargs_option_t *option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);

	if (option->sname == 0 && option->lname == NULL) {
		status.code = CARGS_ERROR_MALFORMED_OPTION;
		status.message = "Option must have a short name or a long name";
		cargs_push_error(cargs, status);
	}
	if (option->flags & ~OPTION_FLAG_MASK) {
		status.code = CARGS_ERROR_INVALID_FLAG;
		cargs_push_error(cargs, status);
	}
	if (option->handler == NULL) {
		status.code = CARGS_ERROR_INVALID_HANDLER;
		status.message = "Option must have a handler";
		cargs_push_error(cargs, status);
	}

	// Check for default value validity with choices
	if (option->choices_count > 0 && option->value.raw != 0)
	{
		bool valid_default = false;
		for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_default) {
			status.code = CARGS_ERROR_INVALID_DEFAULT;
			status.message = "Default value must be one of the available choices";
			cargs_push_error(cargs, status);
		}
	}


	// Check for conflicting dependencies
	if (option->requires != NULL && option->conflicts != NULL)
	{
		for (int i = 0; option->requires[i] != NULL; ++i) {
			for (int j = 0; option->conflicts[j] != NULL; ++j)
			{
				if (strcmp(option->requires[i], option->conflicts[j]) == 0) {
					status.code = CARGS_ERROR_INVALID_DEPENDENCY;
					status.message = "Option cannot require and conflict with the same option";
					cargs_push_error(cargs, status);
	}	}	}	}

	return (status);
}

static cargs_error_t __ensure_group_validity(cargs_t *cargs, cargs_option_t *option)
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

static cargs_error_t __ensure_positional_validity(cargs_t *cargs, cargs_option_t *option)
{
	cargs_error_t status = CARGS_ERROR(
		CARGS_SUCCESS, NULL,
		CARGS_ERROR_CONTEXT(cargs, option)
	);

	if (option->name == NULL) {
		status.code = CARGS_ERROR_MALFORMED_OPTION;
		status.message = "Positional option must have a name";
		cargs_push_error(cargs, status);
	}

	if (option->flags & ~POSITIONAL_FLAG_MASK) {
		status.code = CARGS_ERROR_INVALID_FLAG;
		cargs_push_error(cargs, status);
	}

	if (option->choices_count > 0 && option->value.raw != 0)
	{
		bool valid_default = false;
		for (size_t i = 0; i < option->choices_count && !valid_default; ++i) {
			value_t choice = choices_to_value(option->value_type, option->choices, option->choices_count, i);
			valid_default = (cmp_value(option->value_type, option->value, choice) == 0);
		}
		if (!valid_default) {
			status.code = CARGS_ERROR_INVALID_DEFAULT;
			status.message = "Default value must be one of the available choices";
			cargs_push_error(cargs, status);
		}
	}

	return (status);
}

static cargs_error_t __ensure_subcommand_validity(cargs_t *cargs, cargs_option_t *option)
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

static cargs_error_t __ensure_validity(cargs_t *cargs, cargs_option_t *option)
{
	switch (option->type) {
		case TYPE_OPTION: return __ensure_option_validity(cargs, option);
		case TYPE_POSITIONAL: return __ensure_positional_validity(cargs, option);
		case TYPE_GROUP: return __ensure_group_validity(cargs, option);
		case TYPE_SUBCOMMAND: return __ensure_subcommand_validity(cargs, option);
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

static int __validate_structure(cargs_t *cargs, cargs_option_t *options)
{
	cargs_error_t status;

	for (int i = 0; options[i].type != TYPE_NONE; ++i)
	{
		cargs_option_t *option = &options[i];
		status.context.option_name = option->name;

		status = __ensure_validity(cargs, option);

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
			const cargs_option_t *prev_subcommand = cargs->active_subcommand;
			cargs->active_subcommand = option;
			__validate_structure(cargs, option->subcommand.options);
			cargs->active_subcommand = prev_subcommand;
		}
	}
	return (status.code);
}

cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version, const char *description)
{
	cargs_t cargs = {
		.program_name = program_name,
		.version = version,
		.description = description,
		.options = options,
		.error_stack.count = 0,
	};

	__validate_structure(&cargs, options);
	if (cargs.error_stack.count > 0) {
		fprintf(stderr, "Error while initializing cargs:\n\n");
		cargs_print_error_stack(&cargs);
		exit(EXIT_FAILURE);
	}

	cargs.active_group = NULL;
	cargs.active_subcommand = NULL;

	return (cargs);
}
