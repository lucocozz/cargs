# Référence des fonctions

Cette page fournit une référence complète pour toutes les fonctions de l'API cargs. Ces fonctions vous permettent d'initialiser la bibliothèque, d'analyser les arguments, d'accéder aux valeurs et de gérer les ressources.

!!! abstract "Aperçu"
    L'API cargs est organisée en plusieurs groupes de fonctions :
    
    - **Initialisation et analyse** - Fonctions de base pour la configuration et l'analyse
    - **Accès aux valeurs** - Fonctions pour récupérer les valeurs des options
    - **Accès aux collections** - Fonctions pour travailler avec les tableaux et les mappages
    - **Gestion des sous-commandes** - Fonctions pour gérer les sous-commandes
    - **Fonctions d'affichage** - Fonctions pour générer le texte d'aide et d'utilisation
    - **Gestion des erreurs** - Fonctions pour gérer et signaler les erreurs

## Initialisation et analyse

### cargs_init

Initialise un contexte cargs avec les informations du programme et les options.

```c
cargs_t cargs_init(cargs_option_t *options, const char *program_name, const char *version);
```

**Paramètres :**
- `options` : Tableau d'options de ligne de commande définies avec `CARGS_OPTIONS`
- `program_name` : Nom du programme (utilisé dans les messages d'aide/d'erreur)
- `version` : Chaîne de version

**Retourne :**
- Une structure `cargs_t` initialisée

**Exemple :**
```c
cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
cargs.description = "Description de mon programme génial";
```

!!! tip
    Après l'initialisation, vous pouvez définir des champs supplémentaires comme `description` et `env_prefix` avant l'analyse.

### cargs_parse

Analyse les arguments de la ligne de commande selon les options définies.

```c
int cargs_parse(cargs_t *cargs, int argc, char **argv);
```

**Paramètres :**
- `cargs` : Pointeur vers un contexte cargs initialisé
- `argc` : Nombre d'arguments (de `main`)
- `argv` : Valeurs des arguments (de `main`)

**Retourne :**
- `CARGS_SUCCESS` (0) en cas de succès
- Un code d'erreur non nul en cas d'échec

**Exemple :**
```c
int status = cargs_parse(&cargs, argc, argv);
if (status != CARGS_SUCCESS) {
    return status;
}
```

### cargs_free

Libère les ressources allouées pendant l'analyse.

```c
void cargs_free(cargs_t *cargs);
```

**Paramètres :**
- `cargs` : Pointeur vers le contexte cargs à libérer

**Exemple :**
```c
cargs_free(&cargs);
```

!!! warning
    Appelez toujours `cargs_free()` lorsque vous avez terminé avec un contexte cargs pour éviter les fuites de mémoire.

## Accès aux valeurs

### cargs_get

Récupère la valeur d'une option.

```c
cargs_value_t cargs_get(cargs_t cargs, const char *option_path);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- La valeur de l'option sous forme d'union `cargs_value_t`, ou `{.raw = 0}` si non trouvée

**Exemple :**
```c
const char *output = cargs_get(cargs, "output").as_string;
int port = cargs_get(cargs, "port").as_int;
bool verbose = cargs_get(cargs, "verbose").as_bool;
```

### cargs_is_set

Vérifie si une option a été explicitement définie sur la ligne de commande.

```c
bool cargs_is_set(cargs_t cargs, const char *option_path);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- `true` si l'option a été définie, `false` sinon

**Exemple :**
```c
if (cargs_is_set(cargs, "verbose")) {
    // Mode verbose activé
}
```

### cargs_count

Obtient le nombre de valeurs pour une option (pour les collections).

```c
size_t cargs_count(cargs_t cargs, const char *option_path);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option (format nom ou `sous-commande.nom`)

**Retourne :**
- Nombre de valeurs pour l'option, ou 0 si non trouvée ou pas une collection

**Exemple :**
```c
size_t tags_count = cargs_count(cargs, "tags");
printf("Tags : %zu\n", tags_count);
```

## Accès aux collections

### cargs_array_get

Récupère un élément d'une option de tableau à l'indice spécifié.

```c
cargs_value_t cargs_array_get(cargs_t cargs, const char *option_path, size_t index);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option de tableau
- `index` : Indice de l'élément à récupérer

**Retourne :**
- La valeur à l'indice spécifié, ou `{.raw = 0}` si non trouvée ou indice hors limites

**Exemple :**
```c
const char *first_tag = cargs_array_get(cargs, "tags", 0).as_string;
```

### cargs_map_get

Récupère une valeur d'une option de mapping avec la clé spécifiée.

```c
cargs_value_t cargs_map_get(cargs_t cargs, const char *option_path, const char *key);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option de mapping
- `key` : Clé à rechercher dans le mapping

**Retourne :**
- La valeur associée à la clé, ou `{.raw = 0}` si non trouvée

**Exemple :**
```c
const char *user = cargs_map_get(cargs, "env", "USER").as_string;
int http_port = cargs_map_get(cargs, "ports", "http").as_int;
```

### cargs_array_it

Crée un itérateur pour parcourir efficacement une option de tableau.

```c
cargs_array_it_t cargs_array_it(cargs_t cargs, const char *option_path);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option de tableau

**Retourne :**
- Structure d'itérateur pour le tableau, avec `count=0` si l'option n'est pas trouvée

**Exemple :**
```c
cargs_array_it_t it = cargs_array_it(cargs, "tags");
```

### cargs_array_next

Obtient l'élément suivant d'un itérateur de tableau.

```c
bool cargs_array_next(cargs_array_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de tableau

**Retourne :**
- `true` si une valeur a été récupérée, `false` si fin du tableau

**Exemple :**
```c
cargs_array_it_t it = cargs_array_it(cargs, "tags");
while (cargs_array_next(&it)) {
    printf("Tag : %s\n", it.value.as_string);
}
```

### cargs_array_reset

Réinitialise un itérateur de tableau au début.

```c
void cargs_array_reset(cargs_array_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de tableau à réinitialiser

**Exemple :**
```c
cargs_array_reset(&it);  // Réinitialiser pour commencer une nouvelle itération
```

### cargs_map_it

Crée un itérateur pour parcourir efficacement une option de mapping.

```c
cargs_map_it_t cargs_map_it(cargs_t cargs, const char *option_path);
```

**Paramètres :**
- `cargs` : Le contexte cargs
- `option_path` : Chemin vers l'option de mapping

**Retourne :**
- Structure d'itérateur pour le mapping, avec `count=0` si l'option n'est pas trouvée

**Exemple :**
```c
cargs_map_it_t it = cargs_map_it(cargs, "env");
```

### cargs_map_next

Obtient la paire clé-valeur suivante d'un itérateur de mapping.

```c
bool cargs_map_next(cargs_map_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de mapping

**Retourne :**
- `true` si une paire a été récupérée, `false` si fin du mapping

**Exemple :**
```c
cargs_map_it_t it = cargs_map_it(cargs, "env");
while (cargs_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

### cargs_map_reset

Réinitialise un itérateur de mapping au début.

```c
void cargs_map_reset(cargs_map_it_t *it);
```

**Paramètres :**
- `it` : Itérateur de mapping à réinitialiser

**Exemple :**
```c
cargs_map_reset(&it);  // Réinitialiser pour commencer une nouvelle itération
```

## Gestion des sous-commandes

### cargs_has_command

Vérifie si une sous-commande a été spécifiée sur la ligne de commande.

```c
bool cargs_has_command(cargs_t cargs);
```

**Paramètres :**
- `cargs` : Le contexte cargs

**Retourne :**
- `true` si une sous-commande a été spécifiée, `false` sinon

**Exemple :**
```c
if (cargs_has_command(cargs)) {
    // Une sous-commande a été spécifiée
} else {
    printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
}
```

### cargs_exec

Exécute l'action associée à la sous-commande spécifiée.

```c
int cargs_exec(cargs_t *cargs, void *data);
```

**Paramètres :**
- `cargs` : Pointeur vers le contexte cargs
- `data` : Données optionnelles à passer à l'action de la sous-commande

**Retourne :**
- Code de statut retourné par l'action de la sous-commande
- `CARGS_ERROR_NO_COMMAND` si aucune commande n'a été spécifiée
- `CARGS_ERROR_INVALID_HANDLER` si la commande n'a pas d'action

**Exemple :**
```c
if (cargs_has_command(cargs)) {
    status = cargs_exec(&cargs, NULL);
}
```

## Fonctions d'affichage

### cargs_print_help

Affiche un message d'aide formaté basé sur les options définies.

```c
void cargs_print_help(cargs_t cargs);
```

**Paramètres :**
- `cargs` : Le contexte cargs

**Exemple :**
```c
cargs_print_help(cargs);
```

### cargs_print_usage

Affiche un résumé court de l'utilisation.

```c
void cargs_print_usage(cargs_t cargs);
```

**Paramètres :**
- `cargs` : Le contexte cargs

**Exemple :**
```c
cargs_print_usage(cargs);
```

### cargs_print_version

Affiche les informations de version.

```c
void cargs_print_version(cargs_t cargs);
```

**Paramètres :**
- `cargs` : Le contexte cargs

**Exemple :**
```c
cargs_print_version(cargs);
```

## Gestion des erreurs

### cargs_print_error_stack

Affiche toutes les erreurs dans la pile d'erreurs.

```c
void cargs_print_error_stack(const cargs_t *cargs);
```

**Paramètres :**
- `cargs` : Le contexte cargs

**Exemple :**
```c
if (status != CARGS_SUCCESS) {
    cargs_print_error_stack(&cargs);
    return status;
}
```

### cargs_strerror

Obtient une description textuelle d'un code d'erreur.

```c
const char *cargs_strerror(cargs_error_type_t error);
```

**Paramètres :**
- `error` : Code d'erreur

**Retourne :**
- Description textuelle de l'erreur

**Exemple :**
```c
printf("Erreur : %s\n", cargs_strerror(status));
```

## Exemple complet

Voici un exemple complet montrant le schéma d'utilisation de la fonction principale :

```c
#include "cargs.h"
#include <stdio.h>

// Définir les options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Activer la sortie verbeuse"),
    OPTION_STRING('o', "output", "Fichier de sortie", DEFAULT("output.txt")),
    POSITIONAL_STRING("input", "Fichier d'entrée")
)

int main(int argc, char **argv)
{
    // Initialiser cargs
    cargs_t cargs = cargs_init(options, "example", "1.0.0");
    cargs.description = "Programme exemple utilisant cargs";
    
    // Analyser les arguments
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        cargs_print_error_stack(&cargs);
        return status;
    }
    
    // Accéder aux valeurs
    const char *input = cargs_get(cargs, "input").as_string;
    const char *output = cargs_get(cargs, "output").as_string;
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    
    // Logique de l'application
    if (verbose) {
        printf("Entrée : %s\n", input);
        printf("Sortie : %s\n", output);
    }
    
    // Libérer les ressources
    cargs_free(&cargs);
    return 0;
}
```

## Catégories de fonctions

Pour faciliter la référence, voici un résumé des catégories de fonctions :

| Catégorie | Fonctions |
|----------|-----------|
| **Initialisation** | `cargs_init`, `cargs_parse`, `cargs_free` |
| **Accès aux valeurs** | `cargs_get`, `cargs_is_set`, `cargs_count` |
| **Fonctions de tableau** | `cargs_array_get`, `cargs_array_it`, `cargs_array_next`, `cargs_array_reset` |
| **Fonctions de mapping** | `cargs_map_get`, `cargs_map_it`, `cargs_map_next`, `cargs_map_reset` |
| **Fonctions de sous-commande** | `cargs_has_command`, `cargs_exec` |
| **Fonctions d'affichage** | `cargs_print_help`, `cargs_print_usage`, `cargs_print_version` |
| **Fonctions d'erreur** | `cargs_print_error_stack`, `cargs_strerror` |

## Documentation associée

- [Référence des types](types.md) - Informations détaillées sur les types de données
- [Référence des macros](macros.md) - Liste complète des macros de définition d'options
- [Aperçu de l'API](overview.md) - Aperçu de haut niveau de l'API cargs
