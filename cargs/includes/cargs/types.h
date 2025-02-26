#ifndef CARGS_TYPES_H
#define CARGS_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "cargs/errors.h"

typedef struct cargs_option_s cargs_option_t;
typedef union value_u value_t;
typedef union validator_data_u validator_data_t;

typedef void (*cargs_handler_t)(cargs_option_t*, char*);
typedef void (*cargs_free_handler_t)(cargs_option_t*);
typedef void (*cargs_validator_t)(value_t, validator_data_t);

typedef enum value_type_e {
    VALUE_TYPE_NONE = 0,

    VALUE_TYPE_INT,
    VALUE_TYPE_STRING,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_BOOL,
} value_type_t;

typedef union value_u {
    uintptr_t   raw;
    void*       as_ptr;
    char        as_char;
    int64_t     as_int;
    double      as_float;
    char*       as_string;
    bool        as_bool;
    char**      as_string_array;
    int64_t*    as_int_array;
    double*     as_float_array;
} value_t;

typedef struct range_s {
    int64_t min;
    int64_t max;
} range_t;

typedef union validator_data_u {
    void*       custom;
    range_t     range;
    char**      choices_string;
    int64_t*    choices_int;
    double*     choices_float;
} validator_data_t;

typedef enum option_type_e {
    TYPE_NONE = 0,

    TYPE_OPTION,
    TYPE_GROUP,
    TYPE_POSITIONAL,
    TYPE_SUBCOMMAND,
} option_type_t;

typedef enum option_flags_e {
    FLAG_NONE = 0,

    // Option flags
    FLAG_REQUIRED = 1 << 0,
    FLAG_HIDDEN = 1 << 1,
    FLAG_ADVANCED = 1 << 2,
    FLAG_DEPRECATED = 1 << 3,
    FLAG_EXPERIMENTAL = 1 << 4,
    FLAG_EXIT = 1 << 5,

    // Array type
    FLAG_SORTED = 1 << 6,
    FLAG_UNIQUE = 1 << 7,

    // Group flags
    FLAG_EXCLUSIVE = 1 << 8,

} option_flags_t;


#define OPTION_FLAG_MASK (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_DEPRECATED | FLAG_EXPERIMENTAL | FLAG_EXIT)
#define OPTION_ARRAY_FLAG_MASK (FLAG_SORTED | FLAG_UNIQUE)
#define GROUP_FLAG_MASK (FLAG_EXCLUSIVE)
#define POSITIONAL_FLAG_MASK (FLAG_NONE)
#define SUBCOMMAND_FLAG_MASK (FLAG_NONE)


typedef struct cargs_option_s {
    // Base matadata
    option_type_t   type;

    // Naming metadata
    const char      *name;
    char            sname;
    const char      *lname;
    const char      *help;
    const char      *hint;

    // Value metadata
    value_type_t    value_type;
    value_t         value;
    value_t         choices;
    size_t          choices_count;

    // Callbacks metadata
    cargs_handler_t     handler;
    cargs_free_handler_t free_handler;
    cargs_validator_t   validator;
    validator_data_t    validator_data;
    size_t              validator_data_size;

    // Dependencies metadata
    const char      **requires;
    const char      **conflicts;

    // Flags and state metadata
    option_flags_t  flags;
    bool            is_set;

    // Subcommand metadata
    struct {
        struct cargs_option_s   *options;
        // int                     options_count;
        const char              *description;
    } subcommand;


} cargs_option_t;

typedef struct cargs_s {
	const char		    *program_name;
	const char		    *version;
    const char          *description;
	cargs_option_t	    *options;
	// size_t              options_count;
    cargs_error_stack_t error_stack;

    // fields for active option
    const char         *active_group; //? replace by cargs_option_t *active_group; ?
    const char         *active_subcommand;
} cargs_t;


#endif // CARGS_TYPES_H
