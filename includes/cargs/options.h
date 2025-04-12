/**
 * cargs/options.h - Option definition macros
 *
 * This header provides macros for defining command-line options.
 * These are the primary interface users interact with to specify
 * what options their program accepts.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */
#ifndef CARGS_OPTIONS_H
#define CARGS_OPTIONS_H

#include "cargs/internal/compiler.h"
#include "cargs/types.h"

/* Forward declaration for handler functions */
int flag_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int bool_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int string_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int int_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int float_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int help_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int version_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int default_free(cargs_option_t *option);

int array_string_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int array_int_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int array_float_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int free_array_string_handler(cargs_option_t *option);

int map_string_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int map_int_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int map_float_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int map_bool_handler(cargs_t *cargs, cargs_option_t *option, char *value);
int free_map_string_handler(cargs_option_t *option);
int free_map_int_handler(cargs_option_t *option);
int free_map_float_handler(cargs_option_t *option);
int free_map_bool_handler(cargs_option_t *option);

int range_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int length_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int count_validator(cargs_t *cargs, cargs_option_t *option, validator_data_t data);
int regex_validator(cargs_t *cargs, const char *value, validator_data_t data);

/*
 * Support macro for character to string conversion
 */
#define CHAR_TO_STRING(c) ((char[]){c, '\0'})
// clang-format off
/*
 * Optional option fields macros
 */
#define DEFINE_NAME(lname, sname) ((lname) ? (lname) : CHAR_TO_STRING(sname))
#define DEFAULT(val)            .value = (cargs_value_t){ .raw = (uintptr_t)(val) }, \
                                .default_value = (cargs_value_t){ .raw = (uintptr_t)(val) }, \
                                .is_set = true, \
                                .have_default = true
#define HANDLER(fn)             .handler = (cargs_handler_t)(fn)
#define ACTION(fn)              .action = (cargs_action_t)(fn)
#define FREE_HANDLER(fn)        .free_handler = (cargs_free_handler_t)(fn)
#define HINT(_hint)             .hint = _hint
#define REQUIRES(...)           .requires = (const char*[]){__VA_ARGS__, NULL}
#define CONFLICTS(...)          .conflicts = (const char*[]){__VA_ARGS__, NULL}
#define GROUP_DESC(desc)        .help = desc
#define HELP(desc)              .help = desc
#define FLAGS(_flags)           .flags = _flags
#define ENV_VAR(name)           .env_name = name

/*
 * Validator macros
 */
#define VALIDATOR(fn, data) \
    .validator = (cargs_validator_t)(fn), \
    .validator_data = (validator_data_t){ .custom = (void*)(data) }

#define PRE_VALIDATOR(fn, data) \
    .pre_validator = (cargs_pre_validator_t)(fn), \
    .pre_validator_data = (validator_data_t){ .custom = (void*)(data) }

#define RANGE(min, max) \
    .validator = (cargs_validator_t)range_validator, \
    .validator_data = (validator_data_t){ .range = (range_t){ min, max } }

#define LENGTH(min, max) \
    .validator = (cargs_validator_t)length_validator, \
    .validator_data = (validator_data_t){ .range = (range_t){ min, max } }

#define COUNT(min, max) \
    .validator = (cargs_validator_t)count_validator, \
    .validator_data = (validator_data_t){ .range = (range_t){ min, max } }

#define REGEX(re) \
    .pre_validator = (cargs_pre_validator_t)regex_validator, \
    .pre_validator_data = (validator_data_t){ .regex = (re) }

/*
 * Choice macros for different types
 */
#define CHOICES_INT(...) \
    .choices = (cargs_value_t){ .as_array_int = (long long[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((long long[]){ __VA_ARGS__ }) / sizeof(long long)

#define CHOICES_STRING(...) \
    .choices = (cargs_value_t){ .as_array_string = (char*[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((char*[]){ __VA_ARGS__ }) / sizeof(char*)

#define CHOICES_FLOAT(...) \
    .choices = (cargs_value_t){ .as_float_array = (double[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((double[]){ __VA_ARGS__ }) / sizeof(double)

/*
 * Base option definition macros
 */
#define OPTION_END() (cargs_option_t){ \
    .type = TYPE_NONE, \
    .name = NULL, \
    .value_type = VALUE_TYPE_NONE \
}


#define OPTION_BASE(_short, _long, _value_type, ...)                                          \
    (cargs_option_t) {                                                                        \
        .type = TYPE_OPTION, .name = DEFINE_NAME(_long, _short),                              \
        .sname = _short, .lname = _long, .value_type = _value_type,                           \
        .free_handler = default_free, ##__VA_ARGS__                                           \
    }

#define POSITIONAL_BASE(_name, _value_type, ...)                                        \
    (cargs_option_t) {                                                                         \
        .type = TYPE_POSITIONAL, .name = _name, .value_type = _value_type,      \
        .free_handler = default_free, .flags = FLAG_REQUIRED, ##__VA_ARGS__                    \
    }

#define GROUP_BASE(_name, ...)                                                                 \
    (cargs_option_t) {                                                                         \
        .type = TYPE_GROUP, .name = _name, ##__VA_ARGS__                                       \
    }

#define SUBCOMMAND_BASE(_name, sub_opts, ...)                                                  \
    (cargs_option_t) {                                                                         \
        .type = TYPE_SUBCOMMAND, .name = _name, .sub_options = sub_opts, ##__VA_ARGS__         \
    }

// clang-format on
/*
 * Option type macros
 */
#define OPTION_FLAG(short_name, long_name, ...)                                                    \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_FLAG, HANDLER(flag_handler), __VA_ARGS__)
#define OPTION_BOOL(short_name, long_name, ...)                                                    \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_BOOL, HANDLER(bool_handler), __VA_ARGS__)
#define OPTION_STRING(short_name, long_name, ...)                                                  \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_STRING, HANDLER(string_handler), __VA_ARGS__)
#define OPTION_INT(short_name, long_name, ...)                                                     \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_INT, HANDLER(int_handler), __VA_ARGS__)
#define OPTION_FLOAT(short_name, long_name, ...)                                                   \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_FLOAT, HANDLER(float_handler), __VA_ARGS__)

#define OPTION_ARRAY_STRING(short_name, long_name, ...)                                            \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_STRING, HANDLER(array_string_handler),     \
                FREE_HANDLER(free_array_string_handler), __VA_ARGS__)
#define OPTION_ARRAY_INT(short_name, long_name, ...)                                               \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_INT, HANDLER(array_int_handler),           \
                __VA_ARGS__)
#define OPTION_ARRAY_FLOAT(short_name, long_name, ...)                                             \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_FLOAT, HANDLER(array_float_handler),       \
                __VA_ARGS__)

#define OPTION_MAP_STRING(short_name, long_name, ...)                                              \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_STRING, HANDLER(map_string_handler),         \
                FREE_HANDLER(free_map_string_handler), __VA_ARGS__)
#define OPTION_MAP_INT(short_name, long_name, ...)                                                 \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_INT, HANDLER(map_int_handler),               \
                FREE_HANDLER(free_map_int_handler), __VA_ARGS__)
#define OPTION_MAP_FLOAT(short_name, long_name, ...)                                               \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_FLOAT, HANDLER(map_float_handler),           \
                FREE_HANDLER(free_map_float_handler), __VA_ARGS__)
#define OPTION_MAP_BOOL(short_name, long_name, ...)                                                \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_BOOL, HANDLER(map_bool_handler),             \
                FREE_HANDLER(free_map_bool_handler), __VA_ARGS__)

/*
 * Common options
 */
#define HELP_OPTION(...)                                                                           \
    OPTION_FLAG('h', "help", HELP("Show help message"), HANDLER(help_handler), ##__VA_ARGS__)
#define VERSION_OPTION(...)                                                                        \
    OPTION_FLAG('V', "version", HELP("Show version information"), HANDLER(version_handler),        \
                ##__VA_ARGS__)

/*
 * Group macros
 */
#define GROUP_START(name, ...) GROUP_BASE(name, __VA_ARGS__)
#define GROUP_END()            GROUP_BASE(NULL)

/*
 * Positional argument macros
 */
#define POSITIONAL_STRING(name, ...)                                                               \
    POSITIONAL_BASE(name, VALUE_TYPE_STRING, HANDLER(string_handler), __VA_ARGS__)
#define POSITIONAL_INT(name, ...)                                                                  \
    POSITIONAL_BASE(name, VALUE_TYPE_INT, HANDLER(int_handler), __VA_ARGS__)
#define POSITIONAL_BOOL(name, ...)                                                                 \
    POSITIONAL_BASE(name, VALUE_TYPE_BOOL, HANDLER(bool_handler), __VA_ARGS__)
#define POSITIONAL_FLOAT(name, ...)                                                                \
    POSITIONAL_BASE(name, VALUE_TYPE_FLOAT, HANDLER(float_handler), __VA_ARGS__)

/*
 * Subcommand macro
 */
#define SUBCOMMAND(name, sub_options, ...) SUBCOMMAND_BASE(name, sub_options, __VA_ARGS__)

/*
 * Options array definition macro
 * @param name: Name of the options array
 * @param ...: Option definitions
 */
#define CARGS_OPTIONS(name, ...)                                                                   \
    PRAGMA_DISABLE_VARIADIC_MACROS()                                                               \
    PRAGMA_DISABLE_OVERRIDE()                                                                      \
    PRAGMA_DISABLE_PEDANTIC()                                                                      \
    cargs_option_t name[] = {__VA_ARGS__, OPTION_END()};                                           \
    PRAGMA_RESTORE()                                                                               \
    PRAGMA_RESTORE()                                                                               \
    PRAGMA_RESTORE()

#endif /* CARGS_OPTIONS_H */
