# Sous-commandes

cargs prend en charge les sous-commandes de style Git/Docker, vous permettant de créer des interfaces en ligne de commande avec des commandes distinctes.

!!! abstract "Aperçu"
    Les sous-commandes vous permettent d'organiser les fonctionnalités de votre programme en commandes séparées, chacune avec ses propres options et comportement. Par exemple :
    
    - `git commit`, `git push`, `git pull`
    - `docker run`, `docker build`, `docker pull`
    
    Ce guide couvre les bases de l'implémentation des sous-commandes dans cargs. Pour une utilisation avancée, y compris les sous-commandes imbriquées, consultez le guide [Commandes imbriquées](../advanced/nested-commands.md).

## Concept de base

Les sous-commandes transforment votre programme d'un simple utilitaire en un outil polyvalent avec des commandes spécialisées :

```
my_program [options globales] commande [options de commande] [arguments]
```

Chaque commande peut avoir son propre ensemble d'options et de comportement tout en partageant des options globales communes.

## Implémentation des sous-commandes

Pour implémenter des sous-commandes dans cargs, vous devez :

1. Définir les options pour chaque sous-commande
2. Définir des actions pour chaque sous-commande
3. Référencer les sous-commandes dans vos options principales

### 1. Définir les options des sous-commandes

D'abord, définissez les options pour chacune de vos sous-commandes :

```c
// Options pour la sous-commande "add"
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", "Forcer l'opération d'ajout"),
    POSITIONAL_STRING("file", "Fichier à ajouter")
)

// Options pour la sous-commande "remove"
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", "Supprimer récursivement les répertoires"),
    POSITIONAL_STRING("file", "Fichier à supprimer")
)
```

### 2. Définir les fonctions d'action

Ensuite, définissez des fonctions d'action qui seront appelées lorsque chaque sous-commande sera exécutée :

```c
// Action pour la sous-commande "add"
int add_command(cargs_t *cargs, void *data)
{
    // Accéder aux options de la sous-commande
    const char* file = cargs_get(*cargs, "add.file").as_string;
    bool force = cargs_get(*cargs, "add.force").as_bool;
    
    printf("Ajout du fichier : %s\n", file);
    if (force) printf("  avec l'option force\n");
    
    return 0;
}

// Action pour la sous-commande "remove"
int remove_command(cargs_t *cargs, void *data)
{
    // Accéder aux options de la sous-commande
    const char* file = cargs_get(*cargs, "remove.file").as_string;
    bool recursive = cargs_get(*cargs, "remove.recursive").as_bool;
    
    printf("Suppression du fichier : %s\n", file);
    if (recursive) printf("  récursivement\n");
    
    return 0;
}
```

### 3. Définir les options principales avec les sous-commandes

Enfin, définissez vos options principales et incluez les sous-commandes :

```c
// Options principales avec sous-commandes
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Option globale applicable à toutes les sous-commandes
    OPTION_FLAG('v', "verbose", "Activer la sortie verbeuse"),
    
    // Définir les sous-commandes
    SUBCOMMAND("add", add_options, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Supprimer des fichiers de l'index"), 
               ACTION(remove_command))
)
```

## Traitement des sous-commandes

Dans votre fonction principale, vous devez vérifier si une sous-commande a été spécifiée et exécuter son action :

```c
int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "subcommands_example", "1.0.0");
    cargs.description = "Exemple de sous-commandes";

    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }

    // Vérifier si une sous-commande a été spécifiée
    if (cargs_has_command(cargs)) {
        // Exécuter l'action de la sous-commande
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
    }

    cargs_free(&cargs);
    return status;
}
```

## Accès aux options

### Accès aux options des sous-commandes

Dans une fonction d'action de sous-commande, vous pouvez accéder aux options de deux façons :

=== "Utilisation du chemin absolu"
    ```c
    // Accès avec chemin complet 
    const char* file = cargs_get(*cargs, "add.file").as_string;
    bool force = cargs_get(*cargs, "add.force").as_bool;
    ```

=== "Utilisation du chemin relatif"
    ```c
    // Dans la fonction add_command, vous pouvez utiliser des chemins relatifs
    const char* file = cargs_get(*cargs, "file").as_string;
    bool force = cargs_get(*cargs, "force").as_bool;
    ```

### Accès aux options globales

Les options globales sont accessibles depuis les actions des sous-commandes :

```c
// Accès aux options globales
bool verbose = cargs_get(*cargs, ".verbose").as_bool;
```

Le point initial (`.`) indique que l'option est définie au niveau racine.

## Intégration de l'aide

cargs intègre automatiquement les sous-commandes dans l'affichage de l'aide :

```
subcommands_example v1.0.0

Exemple de sous-commandes

Usage: subcommands_example [OPTIONS] COMMAND

Options:
  -h, --help             - Afficher ce message d'aide (exit)
  -V, --version          - Afficher les informations de version (exit)
  -v, --verbose          - Activer la sortie verbeuse

Commandes:
  add                    - Ajouter des fichiers à l'index
  rm                     - Supprimer des fichiers de l'index

Exécutez 'subcommands_example COMMAND --help' pour plus d'informations sur une commande.
```

Chaque sous-commande a également sa propre aide :

```
subcommands_example v1.0.0

Usage: subcommands_example add [OPTIONS] <file>

Ajouter des fichiers à l'index

Arguments:
  <file>                 - Fichier à ajouter

Options:
  -h, --help             - Afficher ce message d'aide (exit)
  -f, --force            - Forcer l'opération d'ajout
```

## Exemple complet

Voici un exemple complet d'implémentation de sous-commandes de base :

```c
#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Gestionnaires d'action des sous-commandes
int add_command(cargs_t *cargs, void *data);
int remove_command(cargs_t *cargs, void *data);

// Définir les options pour la sous-commande "add"
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", "Forcer l'opération d'ajout"),
    POSITIONAL_STRING("file", "Fichier à ajouter")
)

// Définir les options pour la sous-commande "remove"
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", "Supprimer récursivement les répertoires"),
    POSITIONAL_STRING("file", "Fichier à supprimer")
)

// Définir les options principales avec sous-commandes
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Option globale applicable à toutes les sous-commandes
    OPTION_FLAG('v', "verbose", "Activer la sortie verbeuse"),
    
    // Définir les sous-commandes
    SUBCOMMAND("add", add_options, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Supprimer des fichiers de l'index"), 
               ACTION(remove_command))
)

// Implémentation de la commande "add"
int add_command(cargs_t *cargs, void *data)
{
    (void)data; // Paramètre non utilisé
    
    // Obtenir l'option globale
    bool verbose = cargs_get(*cargs, ".verbose").as_bool;
    
    // Obtenir les options spécifiques à la commande
    const char* file = cargs_get(*cargs, "file").as_string;
    bool force = cargs_get(*cargs, "force").as_bool;

    printf("Ajout du fichier : %s\n", file);
    if (verbose) printf("  mode verbeux activé\n");
    if (force) printf("  avec l'option force\n");

    return 0;
}

// Implémentation de la commande "remove"
int remove_command(cargs_t *cargs, void *data)
{
    (void)data; // Paramètre non utilisé
    
    // Obtenir l'option globale
    bool verbose = cargs_get(*cargs, ".verbose").as_bool;
    
    // Obtenir les options spécifiques à la commande
    const char* file = cargs_get(*cargs, "file").as_string;
    bool recursive = cargs_get(*cargs, "recursive").as_bool;

    printf("Suppression du fichier : %s\n", file);
    if (verbose) printf("  mode verbeux activé\n");
    if (recursive) printf("  récursivement\n");

    return 0;
}

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "subcommands_example", "1.0.0");
    cargs.description = "Exemple de sous-commandes";

    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }

    if (cargs_has_command(cargs)) {
        // Exécuter le gestionnaire de sous-commande
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
    }

    cargs_free(&cargs);
    return 0;
}
```

## Prochaines étapes

Pour des fonctionnalités de sous-commandes plus avancées, telles que :

- Sous-commandes imbriquées (commandes au sein de commandes)
- Abréviations de noms de commandes
- Placements alternatifs d'arguments positionnels
- Gestion de commande personnalisée

Consultez le guide [Commandes imbriquées](../advanced/nested-commands.md).
