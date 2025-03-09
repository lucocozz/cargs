#ifndef CARGS_MACROS_H
#define CARGS_MACROS_H

#include "cargs/types.h"
#include "cargs/utils.h"
#include "cargs/callbacks/handlers.h"
#include "cargs/callbacks/validators.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>


// Optionals options fields

#define DEFINE_NAME(lname, sname) (lname ? lname : CHAR_TO_STRING(sname))
#define DEFAULT(val)            .value = (value_t){ .raw = (uintptr_t)val }, .is_set = true
#define HANDLER(fn)             .handler = (cargs_handler_t)(fn)
#define FREE_HANDLER(fn)        .free_handler = (cargs_free_handler_t)(fn)
#define HINT(_hint)             .hint = _hint
#define REQUIRES(values...)     .requires = (const char*[]){values, NULL}
#define CONFLICTS(values...)    .conflicts = (const char*[]){values, NULL}
#define GROUP_DESC(desc)        .help = desc
#define FLAGS(_flags)           .flags = _flags


#define VALIDATOR_DATA(data, size) \
    .validator_data = (validator_data_t){ .custom = (void*)data }, \
    .validator_data_size = size

#define VALIDATOR(fn, data, size) \
    .validator = (cargs_validator_t)(fn), \
    VALIDATOR_DATA(data, size)

#define RANGE(min, max) \
    .validator = (cargs_validator_t)range_validator, \
    .validator_data = (validator_data_t){ .range = (range_t){ min, max } }, \
    .validator_data_size = sizeof(range_t)


#define CHOICES_INT(values...) \
    .choices = (value_t){ .as_int_array = (int64_t[]){ values } }, \
    .choices_count = sizeof((int64_t[]){ values }) / sizeof(int64_t)

#define CHOICES_STRING(values...) \
    .choices = (value_t){ .as_string_array = (char*[]){ values } }, \
    .choices_count = sizeof((char*[]){ values }) / sizeof(char*)

#define CHOICES_FLOAT(values...) \
    .choices = (value_t){ .as_float_array = (double[]){ values } }, \
    .choices_count = sizeof((double[]){ values }) / sizeof(double)


// Base option macros

#define OPTION_END() (cargs_option_t){ \
    .type = TYPE_NONE, \
    .name = NULL, \
    .value_type = VALUE_TYPE_NONE \
}

#define OPTION_BASE(_short, _long, _help, _value_type,  opts...) (cargs_option_t){ \
    .type = TYPE_OPTION, \
    .name = DEFINE_NAME(_long, _short), \
    .sname = _short, \
    .lname = _long, \
    .help = _help, \
    .value_type = _value_type, \
    ##opts \
}

#define POSITIONAL_BASE(_name, _help, _value_type, opts...) (cargs_option_t){ \
    .type = TYPE_POSITIONAL, \
    .name = _name, \
    .help = _help, \
    .value_type = _value_type, \
    ##opts \
}

#define GROUP_BASE(_name, opts...) (cargs_option_t){ \
    .type = TYPE_GROUP, \
    .name = _name, \
    ##opts \
}

#define SUBCOMMAND_BASE(_name, sub_opts, opts...) (cargs_option_t){ \
    .type = TYPE_SUBCOMMAND, \
    .name = _name, \
    .subcommand = { \
        .options = sub_opts, \
        ##opts \
    } \
}


// Option macros

#define OPTION_FLAG(short_name, long_name, help, options...) \
    OPTION_BASE(short_name, long_name, help, VALUE_TYPE_BOOL, HANDLER(bool_handler), options)
#define OPTION_STRING(short_name, long_name, help, options...) \
    OPTION_BASE(short_name, long_name, help, VALUE_TYPE_STRING, HANDLER(string_handler), options)
#define OPTION_INT(short_name, long_name, help, options...) \
    OPTION_BASE(short_name, long_name, help, VALUE_TYPE_INT, HANDLER(int_handler), options)
#define OPTION_FLOAT(short_name, long_name, help, options...) \
    OPTION_BASE(short_name, long_name, help, VALUE_TYPE_FLOAT, HANDLER(float_handler), options)


// Common options

#define HELP_OPTION(...) \
    OPTION_FLAG('h', "help", "Show help message", HANDLER(help_handler), ##__VA_ARGS__)
#define VERSION_OPTION(...) \
    OPTION_FLAG('V', "version", "Show version information", HANDLER(version_handler), ##__VA_ARGS__)


// Group macros

#define GROUP_START(name, opts...) \
    GROUP_BASE(name, ##opts)
#define GROUP_END() \
    GROUP_BASE(NULL)

/*
#define INCLUDE_GROUP(name, group_options, opts ...) \
    GROUP_START(name, .subcommand.options = group_options, ##opts) \
    GROUP_END()
*/


// Positional arguments

#define POSITIONAL_STRING(name, help, options...) \
    POSITIONAL_BASE(name, help, VALUE_TYPE_STRING, HANDLER(string_handler), options)
#define POSITIONAL_INT(name, help, options...) \
    POSITIONAL_BASE(name, help, VALUE_TYPE_INT, HANDLER(int_handler), options)
#define POSITIONAL_BOOL(name, help, options...) \
    POSITIONAL_BASE(name, help, VALUE_TYPE_BOOL, HANDLER(bool_handler), options)
#define POSITIONAL_FLOAT(name, help, options...) \
    POSITIONAL_BASE(name, help, VALUE_TYPE_FLOAT, HANDLER(float_handler), options)


// Subcommand macros

#define SUBCOMMAND(name, sub_options, opts...) \
    SUBCOMMAND_BASE(name, sub_options, ##opts)


// Options array

#define CARGS_OPTIONS(name, options...) \
    PRAGMA_DISABLE_OVERRIDE() \
    cargs_option_t name[] = { options OPTION_END() }; \
    PRAGMA_RESTORE()


#endif // CARGS_MACROS_H
