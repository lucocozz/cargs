# Options de base

cargs prend en charge différents types d'options pour répondre à divers besoins. Cette page présente les types d'options fondamentaux.

## Types d'options principaux

cargs prend en charge quatre types fondamentaux d'éléments de ligne de commande :

* **Options** - Éléments avec préfixe tiret
    * Avec valeurs (`-o valeur`, `--option=valeur`)
    * Drapeaux/Booléens (`-v`, `--verbose`)
* **Positionnels** - Arguments sans préfixe
    * Requis (doivent être fournis)
    * Optionnels (peuvent être omis)
* **Groupes** - Organisation logique des options
    * Groupes réguliers (regroupement visuel)
    * Groupes exclusifs (un seul peut être sélectionné)
* **Sous-commandes** - Commandes hiérarchiques
    * Commandes simples
    * Commandes imbriquées

## Options avec valeurs

### Option de type chaîne

=== "Définition"
    ```c
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), 
                  DEFAULT("output.txt"),   // Valeur par défaut
                  HINT("FILE"))            // Indice affiché dans l'aide
    ```

=== "Utilisation"
    ```bash
    $ ./my_program --output=file.txt
    $ ./my_program -o file.txt
    ```

=== "Accès"
    ```c
    const char* output = cargs_get(cargs, "output").as_string;
    ```

### Option de type entier

=== "Définition"
    ```c
    OPTION_INT('p', "port", HELP("Numéro de port"), 
               RANGE(1, 65535),   // Validation de plage
               DEFAULT(8080))     // Valeur par défaut
    ```

=== "Utilisation"
    ```bash
    $ ./my_program --port=8080
    $ ./my_program -p 8080
    ```

=== "Accès"
    ```c
    int port = cargs_get(cargs, "port").as_int;
    ```

### Option de type flottant

=== "Définition"
    ```c
    OPTION_FLOAT('s', "scale", HELP("Facteur d'échelle"), 
                 DEFAULT(1.0))    // Valeur par défaut
    ```

=== "Utilisation"
    ```bash
    $ ./my_program --scale=2.5
    $ ./my_program -s 2.5
    ```

=== "Accès"
    ```c
    float scale = cargs_get(cargs, "scale").as_float;
    ```

### Options Booléennes
=== "Définition"
    ```c
    OPTION_BOOL('f', "force", HELP("Forcer l'opération"),
                 DEFAULT(false))  // Default value
    ```
=== "Utilisation"
    ```bash
    $ ./my_program --force=true
    $ ./my_program -f false
    ```
=== "Accès"
    ```c
    bool force = cargs_get(cargs, "force").as_bool;
    ```

## Options Drapeau Booléennes (pas de valeur)

=== "Définition"
    ```c
    OPTION_FLAG('v', "verbose", HELP("Activer le mode verbeux"))
    ```

=== "Utilisation"
    ```bash
    $ ./my_program --verbose
    $ ./my_program -v
    ```

=== "Accès"
    ```c
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    ```

!!! info
    Aucune valeur n'est requise pour les options booléennes ; la présence de l'option active le drapeau.

## Options standard

!!! note "Option d'aide"
    ```c
    HELP_OPTION(FLAGS(FLAG_EXIT))
    ```
    Crée une option `-h, --help` qui affiche automatiquement un message d'aide et quitte le programme.

!!! tip "Option de version"
    ```c
    VERSION_OPTION(FLAGS(FLAG_EXIT))
    ```
    Crée une option `-V, --version` qui affiche la version du programme et quitte.

## Arguments positionnels

Les arguments positionnels sont ordonnés et ne sont pas précédés de tirets.

=== "Positionnel requis"
    ```c
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
    ```

=== "Positionnel optionnel"
    ```c
    POSITIONAL_STRING("output", HELP("Fichier de sortie"), 
                      FLAGS(FLAG_OPTIONAL),   // Argument optionnel
                      DEFAULT("output.txt"))  // Valeur par défaut
    ```

=== "Utilisation"
    ```bash
    $ ./my_program input.txt [output.txt]
    ```

!!! warning "Important"
    Les arguments positionnels requis doivent toujours être définis avant les optionnels dans votre définition `CARGS_OPTIONS`. Cette exigence d'ordre est validée par cargs lors de l'initialisation, et le non-respect de cette règle entraînera une erreur.

Par exemple, voici l'ordre correct :

```c
CARGS_OPTIONS(
    options,
    // Arguments positionnels requis d'abord
    POSITIONAL_STRING("input", HELP("Fichier d'entrée")),                       // Requis
    POSITIONAL_STRING("output", HELP("Fichier de sortie")),                     // Requis
    
    // Arguments positionnels optionnels ensuite
    POSITIONAL_INT("buffer_size", HELP("Taille du tampon"), 
                   FLAGS(FLAG_OPTIONAL), DEFAULT(4096)),                  // Optionnel
    POSITIONAL_STRING("log_file", HELP("Fichier journal"), FLAGS(FLAG_OPTIONAL)) // Optionnel
)
```

## Options avec nom court ou long uniquement

| Type | Définition | Format utilisateur | Code d'accès |
|------|------------|-------------|-------------|
| Nom court uniquement | `OPTION_INT('p', NULL, HELP("Numéro de port"))` | `-p 8080` | `cargs_get(cargs, "p").as_int` |
| Nom long uniquement | `OPTION_FLAG('\0', "dry-run", HELP("Exécuter sans appliquer de changements"))` | `--dry-run` | `cargs_get(cargs, "dry-run").as_bool` |

!!! tip "Accéder aux options"
    Lors de l'accès aux valeurs d'options avec des fonctions comme `cargs_get()`, cargs utilise une règle spécifique :
    
    - Par défaut, cargs utilise le **nom long** comme identifiant
    - Si le nom long n'est pas défini (NULL), il utilise le **nom court** comme identifiant

## Groupes d'options

Les groupes permettent d'organiser visuellement les options dans l'aide :

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    
    GROUP_START("Connection", GROUP_DESC("Options de connexion")),
        OPTION_STRING('h', "host", HELP("Nom d'hôte"), DEFAULT("localhost")),
        OPTION_INT('p', "port", HELP("Numéro de port"), DEFAULT(8080)),
    GROUP_END(),
    
    GROUP_START("Security", GROUP_DESC("Options de sécurité")),
        OPTION_STRING('u', "username", HELP("Nom d'utilisateur")),
        OPTION_STRING('P', "password", HELP("Mot de passe")),
        OPTION_FLAG('s', "secure", HELP("Utiliser une connexion sécurisée")),
    GROUP_END()
)
```

L'aide générée affichera les options organisées par groupes :

```
Options de connexion :
  -h, --host <STR>      - Nom d'hôte (défaut : "localhost")
  -p, --port <NUM>      - Numéro de port (défaut : 8080)

Options de sécurité :
  -u, --username <STR>  - Nom d'utilisateur
  -P, --password <STR>  - Mot de passe
  -s, --secure          - Utiliser une connexion sécurisée
```

## Groupes exclusifs

Vous pouvez créer des groupes d'options mutuellement exclusives :

```c
GROUP_START("Compression", GROUP_DESC("Options de compression"), 
            FLAGS(FLAG_EXCLUSIVE)),
    OPTION_FLAG('z', "gzip", HELP("Utiliser la compression gzip")),
    OPTION_FLAG('j', "bzip2", HELP("Utiliser la compression bzip2")),
    OPTION_FLAG('Z', "lzma", HELP("Utiliser la compression lzma")),
GROUP_END()
```

Avec un groupe exclusif, l'utilisateur ne peut spécifier qu'une seule des options du groupe.

## Drapeaux d'options

Les options peuvent avoir divers drapeaux qui modifient leur comportement :

| Drapeau | Description | Exemple |
|------|-------------|---------|
| `FLAG_REQUIRED` | L'option doit être spécifiée | `FLAGS(FLAG_REQUIRED)` |
| `FLAG_HIDDEN` | L'option est masquée dans l'aide | `FLAGS(FLAG_HIDDEN)` |
| `FLAG_EXIT` | Le programme se termine après traitement | `FLAGS(FLAG_EXIT)` |
| `FLAG_ADVANCED` | L'option est marquée comme avancée | `FLAGS(FLAG_ADVANCED)` |
| `FLAG_DEPRECATED` | L'option est marquée comme obsolète | `FLAGS(FLAG_DEPRECATED)` |
| `FLAG_EXPERIMENTAL` | L'option est marquée comme expérimentale | `FLAGS(FLAG_EXPERIMENTAL)` |

Plusieurs drapeaux peuvent être combinés en utilisant l'opérateur OU binaire :

```c
OPTION_STRING('t', "temp-dir", "Répertoire temporaire",
              FLAGS(FLAG_ADVANCED | FLAG_EXPERIMENTAL))
```

## Accès aux valeurs des options

Après l'analyse des arguments, vous pouvez accéder aux valeurs des options :

```c
// Obtenir une valeur de type chaîne
const char *output = cargs_get(cargs, "output").as_string;

// Obtenir une valeur de type entier
int port = cargs_get(cargs, "port").as_int;

// Obtenir une valeur de type flottant
float scale = cargs_get(cargs, "scale").as_float;

// Vérifier si un drapeau est activé
bool verbose = cargs_get(cargs, "verbose").as_bool;

// Vérifier si une option a été explicitement définie
if (cargs_is_set(cargs, "output")) {
    // L'option output a été spécifiée par l'utilisateur
}
```

## Options avec dépendances

Vous pouvez définir des dépendances entre options :

```c
// L'option username nécessite password
OPTION_STRING('u', "username", HELP("Nom d'utilisateur"), 
              REQUIRES("password"))

// L'option password nécessite username
OPTION_STRING('p', "password", HELP("Mot de passe"), 
              REQUIRES("username"))
```

## Options incompatibles

Vous pouvez définir des incompatibilités entre options :

```c
// L'option verbose est incompatible avec quiet
OPTION_FLAG('v', "verbose", HELP("Mode verbeux"), 
            CONFLICTS("quiet"))

// L'option quiet est incompatible avec verbose
OPTION_FLAG('q', "quiet", HELP("Mode silencieux"), 
            CONFLICTS("verbose"))
```

## Exemple complet

```c
#include "cargs.h"
#include <stdio.h>

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Options standard
    OPTION_FLAG('v', "verbose", HELP("Activer le mode verbeux")),
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Numéro de port"), RANGE(1, 65535), DEFAULT(8080)),
    OPTION_FLOAT('s', "scale", HELP("Facteur d'échelle"), DEFAULT(1.0)),
    
    // Options exclusives
    GROUP_START("Mode", GROUP_DESC("Options de mode"), FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('d', "debug", HELP("Mode débogage")),
        OPTION_FLAG('r', "release", HELP("Mode production")),
    GROUP_END(),
    
    // Options avec dépendances
    OPTION_STRING('u', "username", HELP("Nom d'utilisateur"), REQUIRES("password")),
    OPTION_STRING('P', "password", HELP("Mot de passe"), REQUIRES("username")),
    
    // Arguments positionnels
    POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
)

int main(int argc, char **argv)
{
    cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Accéder aux valeurs
    const char *input = cargs_get(cargs, "input").as_string;
    const char *output = cargs_get(cargs, "output").as_string;
    int port = cargs_get(cargs, "port").as_int;
    float scale = cargs_get(cargs, "scale").as_float;
    bool verbose = cargs_get(cargs, "verbose").as_bool;
    
    // Vérifier les options exclusives
    bool debug = cargs_get(cargs, "debug").as_bool;
    bool release = cargs_get(cargs, "release").as_bool;
    
    // Authentification si spécifiée
    if (cargs_is_set(cargs, "username")) {
        const char *username = cargs_get(cargs, "username").as_string;
        const char *password = cargs_get(cargs, "password").as_string;
        
        printf("Authentification avec %s\n", username);
    }
    
    // Libérer les ressources
    cargs_free(&cargs);
    return 0;
}
```

Lorsqu'il est exécuté avec `--help`, cet exemple générera un affichage d'aide bien formaté avec toutes les options correctement organisées.
