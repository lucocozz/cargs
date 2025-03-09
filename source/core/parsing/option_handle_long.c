#include <stdio.h>
#include <string.h>

#include "cargs/types.h"
#include "cargs/parsing.h"
#include "cargs/utils.h"


int handle_long_option(cargs_t *cargs, cargs_option_t *options, char *arg, char **argv, int argc, int *current_index) 
{
	char option_name[64] = {0};
	char *equal_pos = strchr(arg, '=');
    if (equal_pos != NULL) {
		strncpy(option_name, arg, equal_pos - arg);
    }
	else
		strncpy(option_name, arg, sizeof(option_name) - 1);
	
	cargs_option_t *option = find_option_by_lname(options, option_name);
    if (option == NULL) {
		CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_ARGUMENT,
			"Unknown option: '--%s'", option_name);
    }
	context_set_option(cargs, option);

	char *value = NULL;
	if (option->value_type != VALUE_TYPE_BOOL)
	{
		if (equal_pos != NULL) // Format "--option=value"
    	    value = equal_pos + 1;
		else if (*current_index + 1 < argc) { // Format ["--option", "value"]
			*current_index += 1;
			value = argv[*current_index];
		}
		else {
			CARGS_REPORT_ERROR(cargs, CARGS_ERROR_MISSING_VALUE,
				"Missing value for option: '--%s'", option_name);
		}
	}

	return (execute_callbacks(cargs, option, value));
}
