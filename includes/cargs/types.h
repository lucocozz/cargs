/**
 * cargs/types.h - Public types and data structures
 * 
 * This header defines all the types and structures that are exposed
 * as part of the public API. Users rely on these definitions to
 * configure and use the library.
 * 
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef CARGS_TYPES_H
#define CARGS_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Forward declarations */
typedef struct cargs_s cargs_t;
typedef struct cargs_option_s cargs_option_t;
typedef union value_u value_t;
typedef union validator_data_u validator_data_t;

/**
 * value_type_t - Types of values an option can hold
 */
typedef enum value_type_e {
    VALUE_TYPE_NONE = 0,
    VALUE_TYPE_INT,
    VALUE_TYPE_STRING,
    VALUE_TYPE_FLOAT,
    VALUE_TYPE_BOOL,
} value_type_t;

/**
 * option_type_t - Types of command line elements
 */
typedef enum option_type_e {
    TYPE_NONE = 0,
    TYPE_OPTION,      /* Standard option with - or -- prefix */
    TYPE_GROUP,       /* Logical grouping of options */
    TYPE_POSITIONAL,  /* Positional argument */
    TYPE_SUBCOMMAND,  /* Subcommand with its own options */
} option_type_t;

/**
 * option_flags_t - Flags that modify option behavior
 */
typedef enum option_flags_e {
    FLAG_NONE = 0,
    /* Option flags */
    FLAG_REQUIRED = 1 << 0,      /* Option must be specified */
    FLAG_HIDDEN = 1 << 1,        /* Option is hidden from help */
    FLAG_ADVANCED = 1 << 2,      /* Option is marked as advanced */
    FLAG_DEPRECATED = 1 << 3,    /* Option is marked as deprecated */
    FLAG_EXPERIMENTAL = 1 << 4,  /* Option is marked as experimental */
    FLAG_EXIT = 1 << 5,          /* Program exits after option handler */
    
    /* Array type flags */
    FLAG_SORTED = 1 << 6,        /* Array values are sorted */
    FLAG_UNIQUE = 1 << 7,        /* Array values are unique */
    
    /* Group flags */
    FLAG_EXCLUSIVE = 1 << 8,     /* Only one option in group can be set */
} option_flags_t;

/* Flag masks for validation */
#define OPTION_FLAG_MASK (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_DEPRECATED | FLAG_EXPERIMENTAL | FLAG_EXIT)
#define OPTION_ARRAY_FLAG_MASK (FLAG_SORTED | FLAG_UNIQUE)
#define GROUP_FLAG_MASK (FLAG_EXCLUSIVE)
#define POSITIONAL_FLAG_MASK (FLAG_REQUIRED)
#define SUBCOMMAND_FLAG_MASK (FLAG_NONE)

/**
 * value_u - Union to hold option values of different types
 */
union value_u {
    uintptr_t   raw;
    void*       as_ptr;
    
    char        as_char;
    char*       as_string;
    
    int         as_int;
    int8_t      as_int8;
    int16_t     as_int16;
    int32_t     as_int32;
    int64_t     as_int64;
    
    double      as_float;
    bool        as_bool;
    
    char**      as_string_array;
    int64_t*    as_int_array;
    double*     as_float_array;
};

/**
 * range_t - Min/max range for numeric validation
 */
typedef struct range_s {
    int64_t min;
    int64_t max;
} range_t;

/**
 * validator_data_u - Data used by validator functions
 */
union validator_data_u {
    void*       custom;      /* Custom validator data */
    range_t     range;       /* Range limits */
    
    /* Choices arrays for different types */
    char**      choices_string;
    int64_t*    choices_int;
    double*     choices_float;
};

/* Callback function types */
typedef int  (*cargs_handler_t)(cargs_t*, cargs_option_t*, char*);
typedef void (*cargs_free_handler_t)(cargs_option_t*);
typedef int  (*cargs_validator_t)(cargs_t*, value_t, validator_data_t);
typedef int  (*cargs_action_t)(cargs_t*, void*);

/**
 * cargs_option_s - Defines a command-line option
 */
struct cargs_option_s {
    /* Base metadata */
    option_type_t   type;
    
    /* Naming metadata */
    const char      *name;      /* Internal name used for references */
    char            sname;      /* Single-character name (e.g., -v) */
    const char      *lname;     /* Long name (e.g., --verbose) */
    const char      *help;      /* Help text */
    const char      *hint;      /* Value hint displayed in help */
    
    /* Value metadata */
    value_type_t    value_type;
    value_t         value;
    value_t         default_value;
    bool            is_allocated;
    bool            have_default;
    value_t         choices;
    size_t          choices_count;
    
    /* Callbacks metadata */
    cargs_handler_t         handler;
    cargs_free_handler_t    free_handler;
    cargs_validator_t       validator;
    validator_data_t        validator_data;
    size_t                  validator_data_size;
    
    /* Dependencies metadata */
    const char      **requires;
    const char      **conflicts;
    
    /* Flags and state metadata */
    option_flags_t  flags;
    bool            is_set;
    
    /* Subcommand metadata */
    struct {
        cargs_action_t          action;
        struct cargs_option_s   *options;
        const char              *description;
    } subcommand;
};

/* Maximum depth of nested subcommands */
#ifndef MAX_SUBCOMMAND_DEPTH
#define MAX_SUBCOMMAND_DEPTH 8
#endif

/**
 * Error context - tracks where errors occurred
 */
typedef struct cargs_error_context_s {
    const char *option_name;
    const char *group_name;
    const char *subcommand_name;
} cargs_error_context_t;

/**
 * Error structure - contains error details
 */
#ifndef CARGS_MAX_ERROR_MESSAGE_SIZE
#define CARGS_MAX_ERROR_MESSAGE_SIZE 256
#endif

typedef struct cargs_error_s {
    cargs_error_context_t   context;
    int                     code;
    char                    message[CARGS_MAX_ERROR_MESSAGE_SIZE];
} cargs_error_t;

/**
 * Error stack - contains multiple errors
 */
#ifndef CARGS_MAX_ERRORS_STACK
#define CARGS_MAX_ERRORS_STACK 16
#endif

typedef struct cargs_error_stack_s {
    cargs_error_t   errors[CARGS_MAX_ERRORS_STACK];
    size_t          count;
} cargs_error_stack_t;

/**
 * cargs_s - Main library context
 */
struct cargs_s {
    /* Public fields */
    const char          *program_name;
    const char          *version;
    const char          *description;
    cargs_option_t      *options;
    cargs_error_stack_t error_stack;

    /* Internal fields - do not access directly */
    struct {
        const char              *option;
        const char              *group;
        const cargs_option_t    *subcommand_stack[MAX_SUBCOMMAND_DEPTH];
        size_t                  subcommand_depth;
    } context;
};

#endif /* CARGS_TYPES_H */
