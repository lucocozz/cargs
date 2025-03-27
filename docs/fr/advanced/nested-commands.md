# Commandes imbriquées

Les commandes imbriquées étendent le concept de base des sous-commandes pour créer des interfaces en ligne de commande riches et hiérarchiques avec plusieurs niveaux de commandes et une gestion avancée des commandes.

!!! abstract "Aperçu"
    Ce guide couvre les fonctionnalités avancées d'interface en ligne de commande dans cargs :
    
    - **Hiérarchies de commandes imbriquées** - Plusieurs niveaux de commandes
    - **Abréviations de commandes** - Support pour les noms de commandes raccourcis
    - **Placement flexible des arguments** - Arguments positionnels avant et après les commandes
    - **Accès avancé par chemin** - Différentes façons d'accéder aux options dans des hiérarchies complexes
    
    Pour l'utilisation de base des sous-commandes, consultez le guide [Sous-commandes](../guide/subcommands.md).

## Hiérarchies de commandes imbriquées

Alors que les sous-commandes de base fournissent un seul niveau de hiérarchie de commandes, les commandes imbriquées permettent plusieurs niveaux :

```
my_app service create    # Deux niveaux : "service" et "create"
my_app config set key value    # Deux niveaux avec arguments positionnels
my_app remote add origin https://example.com    # Hiérarchie multi-niveaux
```

Cette fonctionnalité avancée est particulièrement utile pour les applications complexes avec de nombreuses commandes regroupées logiquement, similaires à des outils comme Git, Docker ou Kubernetes.

### Définition des commandes imbriquées

Pour implémenter des commandes imbriquées, vous créez une hiérarchie de définitions de commandes :

```c
// Définir les options pour la commande "service create"
CARGS_OPTIONS(
    service_create_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('n', "name", "Nom du service", FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", "Image du conteneur", FLAGS(FLAG_REQUIRED))
)

// Définir les options pour le groupe de commandes "service"
CARGS_OPTIONS(
    service_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("create", service_create_options, 
               HELP("Créer un nouveau service"), 
               ACTION(service_create_action))
    // Autres sous-commandes de service...
)

// Définir les options principales avec des sous-commandes de premier niveau
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Options globales au niveau racine
    OPTION_FLAG('d', "debug", "Activer le mode debug"),
    
    SUBCOMMAND("service", service_options, 
               HELP("Commandes de gestion de service"))
)
```

## Abréviations de commandes

cargs prend en charge les abréviations de noms de commandes, permettant aux utilisateurs de taper des versions raccourcies de noms de commandes tant qu'elles sont sans ambiguïté :

=== "Définition"
    ```c
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("install", install_options, 
                   HELP("Installer un package")),
        SUBCOMMAND("init", init_options, 
                   HELP("Initialiser la configuration"))
    )
    ```

=== "Exemples d'utilisation"
    ```bash
    # Tous ces exemples sont équivalents :
    my_program install package.tgz
    my_program ins package.tgz
    my_program i package.tgz    # ERREUR : Ambigu (pourrait être "install" ou "init")
    
    # Pour "init", ces commandes sont équivalentes :
    my_program init config.json
    my_program ini config.json
    ```

Ce comportement est similaire à l'abréviation de commande que l'on trouve dans des outils comme `ip` où `ip route add` peut être abrégé en `ip r a`.

!!! note "Abréviations ambiguës"
    Si une abréviation correspond à plusieurs commandes, cargs signalera une erreur.
    
    Par exemple, avec les commandes `status` et `start`, taper `sta` serait ambigu.

### Implémentation de l'abréviation de commande

L'abréviation de commande est intégrée à cargs et ne nécessite pas de configuration spéciale. cargs fait correspondre la commande en trouvant le préfixe unique le plus long :

1. L'utilisateur entre `ip r a 192.168.1.0/24`
2. cargs recherche les commandes commençant par `r` et trouve `route`
3. cargs associe cela à la commande `route`
4. À l'intérieur de la commande `route`, il recherche les commandes commençant par `a` et trouve `add`
5. La commande est traitée comme `ip route add 192.168.1.0/24`

## Placement flexible des arguments positionnels

Dans les structures de commandes complexes, cargs prend en charge un placement flexible des arguments positionnels :

```
program pos1 pos2 command subcmd pos3 pos4
```

Cela permet des structures de commandes intuitives où certains arguments positionnels sont logiques avant la sélection de commande, tandis que d'autres appartiennent à l'après.

=== "Définition"
    ```c
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        // Arguments positionnels globaux
        POSITIONAL_STRING("source", "Répertoire source"),
        
        // Commandes avec leurs propres arguments positionnels
        SUBCOMMAND("copy", copy_options,
                   HELP("Copier des fichiers"))
    )
    
    CARGS_OPTIONS(
        copy_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        POSITIONAL_STRING("destination", "Répertoire de destination")
    )
    ```

=== "Exemple d'utilisation"
    ```bash
    # Utilisations valides :
    my_program /source/dir copy /dest/dir
    my_program copy /dest/dir     # Utilise la source par défaut
    ```

### Accès aux arguments positionnels globaux et spécifiques aux commandes

Lors de l'utilisation d'arguments positionnels à différents niveaux, accédez-y avec les chemins appropriés :

```c
int copy_command(cargs_t *cargs, void *data)
{
    // Argument positionnel global défini avant la commande
    const char* source = cargs_get(*cargs, ".source").as_string;
    
    // Argument positionnel spécifique à la commande
    const char* destination = cargs_get(*cargs, "destination").as_string;
    
    printf("Copie de %s vers %s\n", source, destination);
    return 0;
}
```

## Formats de chemins avancés

Lorsque vous travaillez avec des commandes imbriquées, cargs offre des formats de chemin spéciaux pour accéder aux options :

### 1. Chemin absolu

Un chemin absolu spécifie le chemin complet de l'option depuis la racine :

```c
// Accéder à l'option de n'importe où 
const char* name = cargs_get(*cargs, "service.create.name").as_string;
```

### 2. Chemin relatif

Dans un gestionnaire d'action de sous-commande, vous pouvez utiliser des chemins relatifs :

```c
int service_create_action(cargs_t *cargs, void *data) {
    // "name" se résout automatiquement en "service.create.name"
    const char* name = cargs_get(*cargs, "name").as_string;
    // ...
}
```

### 3. Chemin au niveau racine

Pour accéder aux options définies au niveau racine depuis une sous-commande profondément imbriquée :

```c
int service_create_action(cargs_t *cargs, void *data) {
    // Accéder au drapeau de debug au niveau racine avec le préfixe point
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    // ...
}
```

## Détection et traitement des commandes imbriquées

### Vérification de l'activation des commandes

Pour déterminer quelles commandes sont actives à chaque niveau :

```c
// Vérifier la commande de premier niveau
if (cargs_is_set(cargs, "service")) {
    // Vérifier la commande de second niveau
    if (cargs_is_set(cargs, "service.create")) {
        // "service create" a été utilisée
    }
} else if (cargs_is_set(cargs, "config")) {
    if (cargs_is_set(cargs, "config.set")) {
        // "config set" a été utilisée
    }
}
```

### Partage de contexte entre commandes

Le paramètre `void *data` de `cargs_exec()` vous permet de passer un contexte à tous les gestionnaires de commandes :

```c
typedef struct {
    FILE *log_file;
    config_t *config;
} app_context_t;

int main(int argc, char **argv) {
    // Initialiser le contexte
    app_context_t context = {
        .log_file = fopen("app.log", "w"),
        .config = load_config()
    };
    
    // Initialisation et analyse normales...
    
    if (cargs_has_command(cargs)) {
        // Passer le contexte aux gestionnaires de commandes
        status = cargs_exec(&cargs, &context);
    }
    
    // Nettoyage...
}

// N'importe quel gestionnaire de commande peut accéder au contexte
int service_create_action(cargs_t *cargs, void *data) {
    app_context_t *context = (app_context_t *)data;
    fprintf(context->log_file, "Création du service...\n");
    // ...
}
```

## Création d'interfaces en ligne de commande riches

Avec les commandes imbriquées, vous pouvez créer des structures de commandes complexes comme celles que l'on trouve dans les outils CLI modernes :

=== "Structure similaire à Git"
    ```c
    // Options principales
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        VERSION_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("remote", remote_options, 
                   HELP("Gérer les dépôts distants")),
        SUBCOMMAND("branch", branch_options,
                   HELP("Gérer les branches"))
    )
    
    // Sous-commandes de remote
    CARGS_OPTIONS(
        remote_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("add", remote_add_options,
                   HELP("Ajouter un dépôt distant"), 
                   ACTION(remote_add_action)),
        SUBCOMMAND("remove", remote_remove_options,
                   HELP("Supprimer un dépôt distant"),
                   ACTION(remote_remove_action))
    )
    ```

=== "Structure similaire à Docker"
    ```c
    // Options principales
    CARGS_OPTIONS(
        options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        VERSION_OPTION(FLAGS(FLAG_EXIT)),
        
        // Options globales pour toutes les commandes
        OPTION_FLAG('q', "quiet", "Supprimer la sortie"),
        
        SUBCOMMAND("container", container_options,
                   HELP("Gérer les conteneurs")),
        SUBCOMMAND("image", image_options,
                   HELP("Gérer les images"))
    )
    
    // Sous-commandes de container
    CARGS_OPTIONS(
        container_options,
        HELP_OPTION(FLAGS(FLAG_EXIT)),
        
        SUBCOMMAND("run", container_run_options,
                   HELP("Exécuter un conteneur"),
                   ACTION(container_run_action)),
        SUBCOMMAND("stop", container_stop_options,
                   HELP("Arrêter un conteneur"),
                   ACTION(container_stop_action))
    )
    ```

## Bonnes pratiques

1. **Limiter la profondeur d'imbrication** : Restez à 2-3 niveaux maximum pour l'utilisabilité
2. **Soyez cohérent** : Utilisez des conventions de nommage similaires pour toutes les commandes
3. **Placez les options globales à la racine** : Les options qui s'appliquent à toutes les commandes doivent être au niveau supérieur
4. **Concevez pour l'abréviation** : Assurez-vous que les noms de commandes aient des préfixes distincts
5. **Documentez les structures de commandes** : Documentez clairement la hiérarchie des commandes dans le texte d'aide

## Exemple complet

Voici un exemple complet démontrant des commandes imbriquées avec prise en charge des abréviations :

```c
#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>

// Gestionnaires d'actions
int service_create_action(cargs_t *cargs, void *data);
int service_list_action(cargs_t *cargs, void *data);
int config_set_action(cargs_t *cargs, void *data);
int config_get_action(cargs_t *cargs, void *data);

// Définir les options pour la commande "service create"
CARGS_OPTIONS(
    service_create_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('n', "name", "Nom du service", FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", "Image du conteneur", FLAGS(FLAG_REQUIRED))
)

// Définir les options pour la commande "service list"
CARGS_OPTIONS(
    service_list_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('a', "all", "Afficher tous les services, y compris les arrêtés")
)

// Définir les options pour la commande parente "service"
CARGS_OPTIONS(
    service_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("create", service_create_options, 
               HELP("Créer un nouveau service"), 
               ACTION(service_create_action)),
    SUBCOMMAND("list", service_list_options, 
               HELP("Lister les services"), 
               ACTION(service_list_action))
)

// Définir les options pour la commande "config set"
CARGS_OPTIONS(
    config_set_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_STRING("key", "Clé de configuration"),
    POSITIONAL_STRING("value", "Valeur de configuration")
)

// Définir les options pour la commande "config get"
CARGS_OPTIONS(
    config_get_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    POSITIONAL_STRING("key", "Clé de configuration")
)

// Définir les options pour la commande parente "config"
CARGS_OPTIONS(
    config_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    SUBCOMMAND("set", config_set_options, 
               HELP("Définir une valeur de configuration"), 
               ACTION(config_set_action)),
    SUBCOMMAND("get", config_get_options, 
               HELP("Obtenir une valeur de configuration"), 
               ACTION(config_get_action))
)

// Définir les options principales avec des sous-commandes de premier niveau
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Option globale au niveau racine
    OPTION_FLAG('d', "debug", "Activer le mode debug"),
    OPTION_STRING('o', "output", "Fichier de sortie", DEFAULT("output.log")),
    
    SUBCOMMAND("service", service_options, 
               HELP("Commandes de gestion de service")),
    SUBCOMMAND("config", service_options, 
               HELP("Commandes de configuration"))
)

// Implémentations d'actions de commandes
int service_create_action(cargs_t *cargs, void *data) {
    (void)data;
    
    // Différentes façons d'accéder aux valeurs d'options
    
    // 1. Chemin relatif (relatif à la sous-commande courante)
    const char* name = cargs_get(*cargs, "name").as_string;
    const char* image = cargs_get(*cargs, "image").as_string;
    
    // 2. Chemin absolu (chemin complet depuis la racine)
    const char* name_abs = cargs_get(*cargs, "service.create.name").as_string;
    (void)name_abs;
    
    // 3. Chemin au niveau racine (accès aux options au niveau racine)
    const char* output = cargs_get(*cargs, ".output").as_string;
    bool debug = cargs_get(*cargs, ".debug").as_bool;
    
    printf("Création du service '%s' utilisant l'image '%s'\n", name, image);
    printf("Fichier de sortie : %s\n", output);
    if (debug) printf("Mode debug activé\n");
    
    return 0;
}

// Autres gestionnaires d'actions...

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "nested_commands", "1.0.0");
    cargs.description = "Exemple de sous-commandes imbriquées et d'abréviation de commandes";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    if (cargs_has_command(cargs)) {
        status = cargs_exec(&cargs, NULL);
    } else {
        printf("Aucune commande spécifiée. Utilisez --help pour voir les commandes disponibles.\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
```

## Limitations

- La profondeur d'imbrication maximale est définie par `MAX_SUBCOMMAND_DEPTH` (par défaut : 8)
- Les noms de commandes ne peuvent pas contenir de points (`.`) car ils sont utilisés comme séparateurs de chemin
- Les commandes profondément imbriquées peuvent devenir difficiles à manier pour les utilisateurs

!!! tip "Considérations d'utilisabilité"
    Bien que cargs prenne en charge une imbrication profonde des commandes, garder votre structure de commandes relativement plate avec des noms de commandes bien choisis offre souvent une meilleure expérience utilisateur.

## Ressources associées

- [Guide des sous-commandes](../guide/subcommands.md) - Implémentation de base des sous-commandes
