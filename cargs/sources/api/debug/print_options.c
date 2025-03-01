#include <stdio.h>
#include "cargs/types.h"
#include "cargs/api/debug.h"

// Fonction utilitaire pour afficher le type de valeur
static const char* value_type_to_string(value_type_t type)
{
    switch (type) {
        case VALUE_TYPE_NONE:   return "none";
        case VALUE_TYPE_INT:    return "int";
        case VALUE_TYPE_STRING: return "string";
        case VALUE_TYPE_FLOAT:  return "float";
        case VALUE_TYPE_BOOL:   return "bool";
        default:                return "unknown";
    }
}

// Fonction utilitaire pour afficher les flags
static void print_flags(option_flags_t flags)
{
    printf("Flags: [");
    if (flags & FLAG_REQUIRED) printf(" required");
    if (flags & FLAG_HIDDEN)   printf(" hidden");
    if (flags & FLAG_ADVANCED) printf(" advanced");
    printf(" ]\n");
}

// Fonction utilitaire pour afficher une valeur selon son type
static void print_value(const char* prefix, value_type_t type, value_t value)
{
    printf("%s: ", prefix);
    switch (type) {
        case VALUE_TYPE_INT:
            printf("%ld", value.as_int);
            break;
        case VALUE_TYPE_STRING:
            printf("\"%s\"", value.as_string ? value.as_string : "null");
            break;
        case VALUE_TYPE_FLOAT:
            printf("%f", value.as_float);
            break;
        case VALUE_TYPE_BOOL:
            printf("%s", value.as_bool ? "true" : "false");
            break;
        default:
            printf("none");
    }
    printf("\n");
}

void print_option(cargs_option_t *option)
{
    printf("\n=== Option ===\n");
    printf("Name: %s\n", option->name);
    if (option->sname) printf("Short name: -%c\n", option->sname);
    if (option->lname) printf("Long name: --%s\n", option->lname);
    if (option->help) printf("Help: %s\n", option->help);
    
    printf("Type: %s\n", value_type_to_string(option->value_type));
    print_value("Default value", option->value_type, option->value);
    printf("Is set: %s\n", option->is_set ? "true" : "false");
    
    print_flags(option->flags);
    
    if (option->requires) {
        printf("Requires:");
        for (const char **req = option->requires; *req; req++) {
            printf(" %s", *req);
        }
        printf("\n");
    }
    
    if (option->conflicts) {
        printf("Conflicts:");
        for (const char **conf = option->conflicts; *conf; conf++) {
            printf(" %s", *conf);
        }
        printf("\n");
    }
    
    // Affichage des choix possibles si définis
    if (option->choices_count > 0) {
        printf("Choices: [");
        for (size_t i = 0; i < option->choices_count; i++) {
            switch (option->value_type) {
                case VALUE_TYPE_INT:
                    printf(" %ld", option->choices.as_int_array[i]);
                    break;
                case VALUE_TYPE_STRING:
                    printf(" \"%s\"", option->choices.as_string_array[i]);
                    break;
                default:
                    break;
            }
        }
        printf(" ]\n");
    }
}

void print_subcommand(cargs_option_t *subcommand)
{
    printf("\n=== Subcommand: %s ===\n", subcommand->name);
    if (subcommand->subcommand.description)
        printf("Description: %s\n", subcommand->subcommand.description);
    print_options(subcommand->subcommand.options);
}

void print_group(cargs_option_t *group)
{
    printf("\n=== Group: %s ===\n", group->name);
    if (group->help)
        printf("Description: %s\n", group->help);
}

void print_positional(cargs_option_t *positional)
{
    printf("\n=== Positional Argument ===\n");
    printf("Name: %s\n", positional->name);
    if (positional->help) printf("Help: %s\n", positional->help);
    printf("Type: %s\n", value_type_to_string(positional->value_type));
    print_flags(positional->flags);
}

void print_options(cargs_option_t *options)
{
    printf("\n====== Options Debug Output ======\n");
    for (size_t i = 0; options[i].type != TYPE_NONE; i++) {
        switch (options[i].type) {
            case TYPE_OPTION:
                print_option(&options[i]);
                break;
            case TYPE_GROUP:
                print_group(&options[i]);
                break;
            case TYPE_POSITIONAL:
                print_positional(&options[i]);
                break;
            case TYPE_SUBCOMMAND:
                print_subcommand(&options[i]);
                break;
            default:
                printf("Unknown option type\n");
                break;
        }
    }
    printf("\n==============================\n");
}
