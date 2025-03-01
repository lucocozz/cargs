#include <string.h>
#include <stdio.h>

#include "cargs/utils.h"
#include "cargs/types.h"
#include "cargs/errors.h"
#include "cargs/parsing.h"


// static int __validate_dependencies(cargs_t *cargs, cargs_option_t *options)
// {
//     cargs_error_t status = { CARGS_SUCCESS, NULL };
    
//     // Pour chaque option définie
//     for (int i = 0; options[i].type != TYPE_NONE; ++i)
//     {
//         cargs_option_t *option = &options[i];
        
//         // Si l'option n'a pas été activée, pas besoin de vérifier ses dépendances
//         if (!option->is_set)
//             continue;

//         // Vérifier les dépendances requises
//         if (option->requires != NULL)
//         {
//             for (int j = 0; option->requires[j] != NULL; ++j)
//             {
//                 const char *required_name = option->requires[j];
//                 bool found = false;
                
//                 // Chercher l'option requise dans toutes les options définies
//                 for (int k = 0; options[k].type != TYPE_NONE && !found; ++k)
//                 {
//                     cargs_option_t *other = &options[k];
//                     if ((other->lname && strcmp(other->lname, required_name) == 0) ||
//                         (other->name && strcmp(other->name, required_name) == 0)) {
                        
//                         if (!other->is_set) {
//                             status.code = CARGS_ERROR_MISSING_REQUIRED;
//                             status.message = "Required option missing";
//                             status.context.option_name = option->name;
//                             cargs_push_error(cargs, status);
//                             return -1;
//                         }
//                         found = true;
//                     }
//                 }
                
//                 if (!found) {
//                     status.code = CARGS_ERROR_INVALID_DEPENDENCY;
//                     status.message = "Required option not defined";
//                     status.context.option_name = option->name;
//                     cargs_push_error(cargs, status);
//                     return -1;
//                 }
//             }
//         }
        
//         // Vérifier les conflits
//         if (option->conflicts != NULL) {
//             for (int j = 0; option->conflicts[j] != NULL; ++j) {
//                 const char *conflict_name = option->conflicts[j];
                
//                 // Chercher l'option en conflit dans toutes les options définies
//                 for (int k = 0; options[k].type != TYPE_NONE; ++k) {
//                     cargs_option_t *other = &options[k];
//                     if (((other->lname && strcmp(other->lname, conflict_name) == 0) ||
//                          (other->name && strcmp(other->name, conflict_name) == 0)) &&
//                         other->is_set) {
                            
//                         status.code = CARGS_ERROR_CONFLICTING_OPTIONS;
//                         status.message = "Conflicting options used together";
//                         status.context.option_name = option->name;
//                         cargs_push_error(cargs, status);
//                         return -1;
//                     }
//                 }
//             }
//         }
//     }
    
//     // Vérifier les sous-commandes (récursivement)
//     for (int i = 0; options[i].type != TYPE_NONE; ++i) {
//         if (options[i].type == TYPE_SUBCOMMAND && options[i].is_set) {
//             const cargs_option_t *prev_subcommand = cargs->active_subcommand;
//             cargs->active_subcommand = &options[i];
            
//             int result = __validate_dependencies(cargs, options[i].subcommand.options);
            
//             cargs->active_subcommand = prev_subcommand;
//             if (result != 0)
//                 return result;
//         }
//     }
    
//     return 0;
// }


int cargs_parse(cargs_t *cargs, int argc, char **argv)
{
    int status = parse_args(cargs, cargs->options, argc - 1, &argv[1]);
    if (status != CARGS_SUCCESS)
        return (status);

    // return __validate_dependencies(cargs, cargs->options);
    return 0;
}
