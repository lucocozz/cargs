# Collections multi-valeurs

cargs prend en charge les collections multi-valeurs à travers les tableaux et les mappages, vous permettant de gérer plusieurs valeurs pour une seule option ou des paires clé-valeur.

!!! abstract "Aperçu"
    Ce guide couvre les bases des collections multi-valeurs dans cargs :
    
    - **Options de type tableau** - Collecte de plusieurs valeurs
    - **Options de type mapping** - Collecte de paires clé-valeur
    - **Formats d'entrée** - Différentes façons pour les utilisateurs de fournir des données de collection
    - **Accès aux valeurs** - Comment récupérer et utiliser les données de collection
    
    Pour une utilisation avancée, consultez le guide [Multi-valeurs avancé](../advanced/multi-values.md).

## Options de type tableau

Les options de type tableau permettent aux utilisateurs de fournir plusieurs valeurs pour une seule option, soit à travers plusieurs occurrences, soit par des listes séparées par des virgules.

### Types de tableaux

cargs prend en charge ces types de tableaux :

- `OPTION_ARRAY_STRING` : Tableau de chaînes
- `OPTION_ARRAY_INT` : Tableau d'entiers
- `OPTION_ARRAY_FLOAT` : Tableau de valeurs à virgule flottante

### Définition des options de type tableau

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Tableau de chaînes simple
    OPTION_ARRAY_STRING('n', "names", "Liste de noms"),
    
    // Tableau d'entiers
    OPTION_ARRAY_INT('i', "ids", "Liste d'identifiants"),
    
    // Tableau de flottants
    OPTION_ARRAY_FLOAT('f', "factors", "Facteurs d'échelle")
)
```

### Formats d'entrée pour les tableaux

Les utilisateurs peuvent fournir des valeurs de tableau de plusieurs façons :

=== "Occurrences multiples d'options"
    ```bash
    ./program --names=john --names=alice --names=bob
    ```

=== "Valeurs séparées par des virgules"
    ```bash
    ./program --names=john,alice,bob
    ```

=== "Approches combinées"
    ```bash
    ./program --names=john,alice --names=bob
    ```

=== "Format d'option courte"
    ```bash
    ./program -n john -n alice -n bob
    ./program -njohn,alice,bob
    ```

### Syntaxe de plage d'entiers

Pour les tableaux d'entiers, cargs prend en charge une syntaxe de plage pratique :

```bash
./program --ids=1-5,10,15-20
# Équivalent à : --ids=1,2,3,4,5,10,15,16,17,18,19,20
```

## Options de type mapping

Les options de type mapping permettent aux utilisateurs de fournir des paires clé-valeur, permettant une configuration structurée via des arguments de ligne de commande.

### Types de mappings

cargs prend en charge ces types de mappings :

- `OPTION_MAP_STRING` : Mapping avec valeurs de type chaîne
- `OPTION_MAP_INT` : Mapping avec valeurs de type entier
- `OPTION_MAP_FLOAT` : Mapping avec valeurs de type flottant
- `OPTION_MAP_BOOL` : Mapping avec valeurs de type booléen

### Définition des options de type mapping

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    // Mapping de chaînes
    OPTION_MAP_STRING('e', "env", "Variables d'environnement"),
    
    // Mapping d'entiers
    OPTION_MAP_INT('p', "ports", "Numéros de port de service"),
    
    // Mapping de flottants
    OPTION_MAP_FLOAT('s', "scales", "Facteurs d'échelle"),
    
    // Mapping de booléens
    OPTION_MAP_BOOL('f', "features", "Indicateurs de fonctionnalités")
)
```

### Formats d'entrée pour les mappings

Les utilisateurs peuvent fournir des valeurs de mapping de plusieurs façons :

=== "Occurrences multiples d'options"
    ```bash
    ./program --env=USER=alice --env=HOME=/home/alice
    ```

=== "Paires clé-valeur séparées par des virgules"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice,TERM=xterm
    ```

=== "Approches combinées"
    ```bash
    ./program --env=USER=alice,HOME=/home/alice --env=TERM=xterm
    ```

=== "Format d'option courte"
    ```bash
    ./program -e USER=alice -e HOME=/home/alice
    ./program -eUSER=alice,HOME=/home/alice
    ```

### Traitement spécial pour les mappings booléens

Pour les mappings booléens (`OPTION_MAP_BOOL`), les valeurs sont analysées comme des booléens :

- **Valeurs vraies** : "true", "yes", "1", "on", "y" (insensible à la casse)
- **Valeurs fausses** : "false", "no", "0", "off", "n" (insensible à la casse)

```bash
./program --features=debug=true,logging=yes,metrics=1,cache=false
```

## Accès aux collections multi-valeurs

cargs fournit plusieurs façons d'accéder aux données de collection.

### Accès aux tableaux de base

Pour accéder aux éléments du tableau :

```c
// Vérifier si l'option de tableau a été définie
if (cargs_is_set(cargs, "names")) {
    // Obtenir le nombre d'éléments du tableau
    size_t count = cargs_count(cargs, "names");
    
    // Obtenir le pointeur du tableau
    value_t *names_array = cargs_get(cargs, "names").as_array;
    
    // Afficher tous les noms
    printf("Noms (%zu) :\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  %zu: %s\n", i+1, names_array[i].as_string);
    }
}
```

### Accès aux mappings de base

Pour accéder aux entrées de mapping :

```c
// Vérifier si l'option de mapping a été définie
if (cargs_is_set(cargs, "env")) {
    // Obtenir le nombre d'éléments du mapping
    size_t count = cargs_count(cargs, "env");
    
    // Obtenir le pointeur du mapping
    cargs_pair_t *env_map = cargs_get(cargs, "env").as_map;
    
    // Afficher toutes les variables d'environnement
    printf("Variables d'environnement (%zu) :\n", count);
    for (size_t i = 0; i < count; i++) {
        const char* key = env_map[i].key;
        const char* value = env_map[i].value.as_string;
        printf("  %s = %s\n", key, value);
    }
}
```

### Assistants d'accès aux éléments

Pour un accès plus pratique à des éléments spécifiques :

```c
// Obtenir un élément spécifique du tableau par indice
const char* first_name = cargs_array_get(cargs, "names", 0).as_string;
int second_id = cargs_array_get(cargs, "ids", 1).as_int;

// Rechercher une valeur spécifique dans un mapping par clé
const char* user = cargs_map_get(cargs, "env", "USER").as_string;
int http_port = cargs_map_get(cargs, "ports", "http").as_int;
bool debug_enabled = cargs_map_get(cargs, "features", "debug").as_bool;
```

Ces fonctions auxiliaires gèrent les indices invalides ou les clés manquantes avec élégance, retournant une valeur vide lorsque l'élément demandé n'existe pas.

## Cas d'utilisation courants

### Étiquettes ou libellés en ligne de commande

```c
OPTION_ARRAY_STRING('t', "tag", "Étiquettes pour la ressource")
```

Utilisation :
```bash
./program --tag=important,urgent --tag=priority
```

### Configuration de service

```c
OPTION_MAP_INT('p', "port", "Mappages de ports pour les services")
```

Utilisation :
```bash
./program --port=http=80,https=443,ftp=21
```

### Bascules de fonctionnalités

```c
OPTION_MAP_BOOL('f', "feature", "Bascules de fonctionnalités")
```

Utilisation :
```bash
./program --feature=dark-mode=on,animations=off,beta=true
```

## Exemple complet

Voici un exemple complet démontrant à la fois les options de type tableau et de type mapping :

```c
#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Options de type tableau
    OPTION_ARRAY_STRING('n', "name", "Noms d'utilisateurs"),
    OPTION_ARRAY_INT('i', "id", "Identifiants d'utilisateurs"),
    
    // Options de type mapping
    OPTION_MAP_STRING('e', "env", "Variables d'environnement"),
    OPTION_MAP_INT('p', "port", "Mappages de ports"),
    OPTION_MAP_BOOL('f', "feature", "Indicateurs de fonctionnalités")
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "multi_values", "1.0.0");
    cargs.description = "Exemple de collections multi-valeurs";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Traiter le tableau de chaînes
    if (cargs_is_set(cargs, "name")) {
        size_t count = cargs_count(cargs, "name");
        value_t *names = cargs_get(cargs, "name").as_array;
        
        printf("Noms (%zu) :\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu: %s\n", i+1, names[i].as_string);
        }
        printf("\n");
    }
    
    // Traiter le tableau d'entiers
    if (cargs_is_set(cargs, "id")) {
        size_t count = cargs_count(cargs, "id");
        value_t *ids = cargs_get(cargs, "id").as_array;
        
        printf("Identifiants (%zu) :\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %zu: %d\n", i+1, ids[i].as_int);
        }
        printf("\n");
    }
    
    // Traiter le mapping de chaînes
    if (cargs_is_set(cargs, "env")) {
        size_t count = cargs_count(cargs, "env");
        cargs_pair_t *env = cargs_get(cargs, "env").as_map;
        
        printf("Variables d'environnement (%zu) :\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s = %s\n", env[i].key, env[i].value.as_string);
        }
        printf("\n");
    }
    
    // Traiter le mapping d'entiers
    if (cargs_is_set(cargs, "port")) {
        size_t count = cargs_count(cargs, "port");
        cargs_pair_t *ports = cargs_get(cargs, "port").as_map;
        
        printf("Mappages de ports (%zu) :\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s: %d\n", ports[i].key, ports[i].value.as_int);
        }
        printf("\n");
    }
    
    // Traiter le mapping booléen
    if (cargs_is_set(cargs, "feature")) {
        size_t count = cargs_count(cargs, "feature");
        cargs_pair_t *features = cargs_get(cargs, "feature").as_map;
        
        printf("Indicateurs de fonctionnalités (%zu) :\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  %s: %s\n", features[i].key, 
                   features[i].value.as_bool ? "activé" : "désactivé");
        }
        printf("\n");
    }
    
    cargs_free(&cargs);
    return 0;
}
```

## Prochaines étapes

Pour une gestion de collection plus avancée incluant :

- Itérateurs pour un parcours efficace
- Drapeaux de collection (tri, unicité)
- Considérations de performance
- Cas d'utilisation avancés

Consultez le guide [Multi-valeurs avancé](../advanced/multi-values.md).
