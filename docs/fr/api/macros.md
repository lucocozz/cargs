# Référence des macros

Cette page fournit une référence complète pour toutes les macros disponibles dans cargs pour définir des options et personnaliser leur comportement.

!!! abstract "Aperçu"
    cargs utilise une approche basée sur des macros pour définir les options de ligne de commande, ce qui permet de spécifier les propriétés des options dans un style déclaratif concis. Les macros sont organisées en plusieurs catégories :
    
    - **Macros de définition** - Définissent des options de différents types
    - **Propriétés d'options** - Personnalisent le comportement des options
    - **Groupes et sous-commandes** - Organisent les options et créent des hiérarchies de commandes
    - **Macros de validation** - Ajoutent des contraintes de validation
    - **Combinaisons de drapeaux** - Configurent les drapeaux d'options

## Macro de définition principale

### CARGS_OPTIONS

C'est la macro principale qui définit un ensemble d'options de ligne de commande :

```c
CARGS_OPTIONS(nom, ...)
```

**Paramètres :**
- `nom` : Nom de la variable qui contiendra le tableau d'options
- `...` : Liste d'options définies avec les macros ci-dessous

**Exemple :**
```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('o', "output", HELP("Fichier de sortie"), DEFAULT("output.txt")),
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse"))
)
```

## Macros de définition d'options

### Options standard

#### OPTION_STRING

Définit une option de type chaîne :

```c
OPTION_STRING(nom_court, nom_long, aide, ...)
```

**Paramètres :**
- `nom_court` : Caractère pour le nom court (ex., 'o' pour -o), ou '\0' si aucun
- `nom_long` : Chaîne pour le nom long (ex., "output" pour --output), ou NULL si aucun
- `aide` : Texte d'aide décrivant l'option
- `...` : Modificateurs optionnels (DEFAULT, HINT, FLAGS, etc.)

**Exemple :**
```c
OPTION_STRING('o', "output", HELP("Fichier de sortie"), 
             DEFAULT("output.txt"), 
             HINT("FILE"))
```

#### OPTION_INT

Définit une option de type entier :

```c
OPTION_INT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_INT('p', "port", HELP("Numéro de port"), 
           DEFAULT(8080), 
           RANGE(1, 65535))
```

#### OPTION_FLOAT

Définit une option de type flottant :

```c
OPTION_FLOAT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_FLOAT('f', "factor", HELP("Facteur d'échelle"), 
             DEFAULT(1.0))
```

#### OPTION_BOOL
Définit une option de type booléen :

```c
OPTION_BOOL(nom_court, nom_long, aide, ...)
```
**Paramètres :** Identiques à `OPTION_STRING`
**Exemple :**
```c
OPTION_BOOL('d', "debug", HELP("Activer le mode débogage"), 
            DEFAULT(false))
```

#### OPTION_FLAG

Définit une option booléenne (drapeau) qui ne prend pas de valeur :

```c
OPTION_FLAG(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse"))
```

### Options de type tableau

#### OPTION_ARRAY_STRING

Définit une option acceptant un tableau de chaînes :

```c
OPTION_ARRAY_STRING(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_ARRAY_STRING('t', "tags", HELP("Tags pour la ressource"), 
                   FLAGS(FLAG_SORTED | FLAG_UNIQUE))
```

#### OPTION_ARRAY_INT

Définit une option acceptant un tableau d'entiers :

```c
OPTION_ARRAY_INT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_ARRAY_INT('p', "ports", HELP("Numéros de ports"), 
                FLAGS(FLAG_UNIQUE))
```

#### OPTION_ARRAY_FLOAT

Définit une option acceptant un tableau de valeurs flottantes :

```c
OPTION_ARRAY_FLOAT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_ARRAY_FLOAT('f', "factors", HELP("Facteurs d'échelle"))
```

### Options de type map

#### OPTION_MAP_STRING

Définit une option acceptant une map de chaînes :

```c
OPTION_MAP_STRING(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_MAP_STRING('e', "env", HELP("Variables d'environnement"), 
                 FLAGS(FLAG_SORTED_KEY))
```

#### OPTION_MAP_INT

Définit une option acceptant une map d'entiers :

```c
OPTION_MAP_INT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_MAP_INT('p', "ports", HELP("Ports de service"), 
              FLAGS(FLAG_SORTED_KEY))
```

#### OPTION_MAP_FLOAT

Définit une option acceptant une map de valeurs flottantes :

```c
OPTION_MAP_FLOAT(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_MAP_FLOAT('s', "scales", HELP("Facteurs d'échelle par dimension"))
```

#### OPTION_MAP_BOOL

Définit une option acceptant une map de booléens :

```c
OPTION_MAP_BOOL(nom_court, nom_long, aide, ...)
```

**Paramètres :** Identiques à `OPTION_STRING`

**Exemple :**
```c
OPTION_MAP_BOOL('f', "features", HELP("Bascules de fonctionnalités"))
```

### Arguments positionnels

#### POSITIONAL_STRING

Définit un argument positionnel de type chaîne :

```c
POSITIONAL_STRING(nom, aide, ...)
```

**Paramètres :**
- `nom` : Nom de l'argument (pour les références et l'affichage d'aide)
- `aide` : Texte d'aide décrivant l'argument
- `...` : Modificateurs optionnels (FLAGS, DEFAULT, etc.)

**Exemple :**
```c
POSITIONAL_STRING("input", HELP("Fichier d'entrée"))
```

#### POSITIONAL_INT

Définit un argument positionnel de type entier :

```c
POSITIONAL_INT(nom, aide, ...)
```

**Paramètres :** Identiques à `POSITIONAL_STRING`

**Exemple :**
```c
POSITIONAL_INT("count", HELP("Nombre d'itérations"), 
               DEFAULT(1))
```

#### POSITIONAL_FLOAT

Définit un argument positionnel de type flottant :

```c
POSITIONAL_FLOAT(nom, aide, ...)
```

**Paramètres :** Identiques à `POSITIONAL_STRING`

**Exemple :**
```c
POSITIONAL_FLOAT("threshold", HELP("Seuil de détection"), 
                 DEFAULT(0.5))
```

#### POSITIONAL_BOOL

Définit un argument positionnel de type booléen :

```c
POSITIONAL_BOOL(nom, aide, ...)
```

**Paramètres :** Identiques à `POSITIONAL_STRING`

**Exemple :**
```c
POSITIONAL_BOOL("enabled", HELP("Activer la fonctionnalité"))
```

### Options courantes

#### HELP_OPTION

Définit une option d'aide (-h, --help) :

```c
HELP_OPTION(...)
```

**Paramètres :**
- `...` : Modificateurs optionnels (typiquement FLAGS(FLAG_EXIT))

**Exemple :**
```c
HELP_OPTION(FLAGS(FLAG_EXIT))
```

#### VERSION_OPTION

Définit une option de version (-V, --version) :

```c
VERSION_OPTION(...)
```

**Paramètres :**
- `...` : Modificateurs optionnels (typiquement FLAGS(FLAG_EXIT))

**Exemple :**
```c
VERSION_OPTION(FLAGS(FLAG_EXIT))
```

### Options de base

#### OPTION_BASE

Définit une option personnalisée avec plus de contrôle :

```c
OPTION_BASE(nom_court, nom_long, aide, type_valeur, ...)
```

**Paramètres :**
- `nom_court` : Caractère pour le nom court (ex., 'o' pour -o), ou '\0' si aucun
- `nom_long` : Chaîne pour le nom long (ex., "output" pour --output), ou NULL si aucun
- `aide` : Texte d'aide décrivant l'option
- `type_valeur` : Type de valeur (VALUE_TYPE_STRING, VALUE_TYPE_INT, etc.)
- `...` : Modificateurs optionnels (HANDLER, FREE_HANDLER, etc.)

**Exemple :**
```c
OPTION_BASE('i', "ip", HELP("Adresse IP"), VALUE_TYPE_CUSTOM,
            HANDLER(ip_handler),
            FREE_HANDLER(ip_free_handler))
```

#### POSITIONAL_BASE

Définit un argument positionnel personnalisé :

```c
POSITIONAL_BASE(nom, aide, type_valeur, ...)
```

**Paramètres :**
- `nom` : Nom de l'argument
- `aide` : Texte d'aide décrivant l'argument
- `type_valeur` : Type de valeur
- `...` : Modificateurs optionnels

**Exemple :**
```c
POSITIONAL_BASE("coordinate", HELP("Coordonnées du point"), VALUE_TYPE_CUSTOM,
                HANDLER(coordinate_handler),
                FREE_HANDLER(coordinate_free_handler))
```

## Macros de groupe et de sous-commande

### GROUP_START

Commence un groupe d'options :

```c
GROUP_START(nom, ...)
```

**Paramètres :**
- `nom` : Nom du groupe
- `...` : Modificateurs optionnels (GROUP_DESC, FLAGS, etc.)

**Exemple :**
```c
GROUP_START("Connection", GROUP_DESC("Options de connexion"))
```

### GROUP_END

Termine un groupe d'options :

```c
GROUP_END()
```

**Exemple :**
```c
GROUP_END()
```

### SUBCOMMAND

Définit une sous-commande avec ses propres options :

```c
SUBCOMMAND(nom, sous_options, ...)
```

**Paramètres :**
- `nom` : Nom de la sous-commande
- `sous_options` : Tableau d'options pour la sous-commande
- `...` : Modificateurs optionnels (HELP, ACTION, etc.)

**Exemple :**
```c
SUBCOMMAND("add", add_options, 
           HELP("Ajouter des fichiers"), 
           ACTION(add_command))
```

## Macros de propriétés

### DEFAULT

Définit une valeur par défaut pour une option :

```c
DEFAULT(valeur)
```

**Exemple :**
```c
OPTION_INT('p', "port", "Numéro de port", DEFAULT(8080))
```

### HINT

Définit un indice affiché dans l'aide pour le format de la valeur :

```c
HINT(texte)
```

**Exemple :**
```c
OPTION_STRING('o', "output", "Fichier de sortie", HINT("FICHIER"))
```

### HELP

Définit le texte d'aide pour une sous-commande :

```c
HELP(texte)
```

**Exemple :**
```c
SUBCOMMAND("add", add_options, HELP("Ajouter des fichiers au dépôt"))
```

### GROUP_DESC

Définit la description pour un groupe d'options :

```c
GROUP_DESC(texte)
```

**Exemple :**
```c
GROUP_START("Advanced", GROUP_DESC("Options de configuration avancées"))
```

### FLAGS

Définit des drapeaux pour modifier le comportement d'une option :

```c
FLAGS(drapeaux)
```

**Exemple :**
```c
OPTION_STRING('o', "output", "Fichier de sortie", FLAGS(FLAG_REQUIRED))
```

### REQUIRES

Définit des dépendances entre options :

```c
REQUIRES(...)
```

**Exemple :**
```c
OPTION_STRING('u', "username", "Nom d'utilisateur", REQUIRES("password"))
```

### CONFLICTS

Définit des incompatibilités entre options :

```c
CONFLICTS(...)
```

**Exemple :**
```c
OPTION_FLAG('v', "verbose", "Mode verbeux", CONFLICTS("quiet"))
```

### ENV_VAR

Spécifie une variable d'environnement associée à l'option :

```c
ENV_VAR(nom)
```

**Exemple :**
```c
OPTION_STRING('H', "host", "Nom d'hôte", ENV_VAR("HOST"))
```

### ACTION

Associe une fonction d'action à une sous-commande :

```c
ACTION(fonction)
```

**Exemple :**
```c
SUBCOMMAND("add", add_options, ACTION(add_command))
```

### HANDLER

Spécifie un gestionnaire personnalisé pour une option :

```c
HANDLER(fonction)
```

**Exemple :**
```c
HANDLER(ip_handler)
```

### FREE_HANDLER

Spécifie un gestionnaire de libération personnalisé pour une option :

```c
FREE_HANDLER(fonction)
```

**Exemple :**
```c
FREE_HANDLER(ip_free_handler)
```

## Macros de validation

### RANGE

Définit une plage valide de valeurs pour une option numérique :

```c
RANGE(min, max)
```

**Exemple :**
```c
OPTION_INT('p', "port", "Numéro de port", RANGE(1, 65535))
```

### CHOICES_STRING, CHOICES_INT, CHOICES_FLOAT

Définit des choix valides pour une option :

```c
CHOICES_STRING(...)
CHOICES_INT(...)
CHOICES_FLOAT(...)
```

**Exemple :**
```c
OPTION_STRING('l', "level", "Niveau de journalisation", 
             CHOICES_STRING("debug", "info", "warning", "error"))
```

### REGEX

Applique une validation par expression régulière :

```c
REGEX(modèle)
```

**Exemple :**
```c
OPTION_STRING('e', "email", "Adresse email", REGEX(CARGS_RE_EMAIL))
```

### MAKE_REGEX

Crée un modèle d'expression régulière avec explication :

```c
MAKE_REGEX(modèle, indice)
```

**Exemple :**
```c
REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000"))
```

### VALIDATOR

Associe un validateur personnalisé à une option :

```c
VALIDATOR(fonction, données)
```

**Exemple :**
```c
VALIDATOR(even_validator, NULL)
```

### PRE_VALIDATOR

Associe un pré-validateur personnalisé à une option :

```c
PRE_VALIDATOR(fonction, données)
```

**Exemple :**
```c
PRE_VALIDATOR(length_validator, &min_length)
```

## Constantes de drapeaux

Ces constantes peuvent être combinées avec la macro `FLAGS()` :

### Drapeaux d'options

| Drapeau | Description |
|------|-------------|
| `FLAG_REQUIRED` | L'option doit être spécifiée |
| `FLAG_HIDDEN` | L'option est masquée dans l'aide |
| `FLAG_ADVANCED` | L'option est marquée comme avancée |
| `FLAG_DEPRECATED` | L'option est marquée comme obsolète |
| `FLAG_EXPERIMENTAL` | L'option est marquée comme expérimentale |
| `FLAG_EXIT` | Le programme se termine après le traitement de cette option |
| `FLAG_ENV_OVERRIDE` | La variable d'environnement peut remplacer la valeur de la ligne de commande |
| `FLAG_AUTO_ENV` | Générer automatiquement le nom de la variable d'environnement |
| `FLAG_NO_ENV_PREFIX` | Ne pas utiliser le préfixe de variable d'environnement |

### Drapeaux de tableau et de map

| Drapeau | Description |
|------|-------------|
| `FLAG_SORTED` | Trier les valeurs du tableau |
| `FLAG_UNIQUE` | Supprimer les valeurs dupliquées du tableau |
| `FLAG_SORTED_KEY` | Trier les entrées de la map par clé |
| `FLAG_SORTED_VALUE` | Trier les entrées de la map par valeur |
| `FLAG_UNIQUE_VALUE` | S'assurer que la map n'a pas de valeurs dupliquées |

### Drapeaux de groupe

| Drapeau | Description |
|------|-------------|
| `FLAG_EXCLUSIVE` | Une seule option du groupe peut être sélectionnée |

### Drapeau d'aide

| Drapeau | Description |
|------|-------------|
| `FLAG_OPTIONAL` | Pour les arguments positionnels uniquement, les marque comme optionnels |

## Exemple complet

Voici un exemple complet montrant diverses macros en utilisation :

```c
// Définir les options pour la sous-commande "add"
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", HELP("Forcer l'opération d'ajout"), 
               CONFLICTS("dry-run")),
    POSITIONAL_STRING("file", HELP("Fichier à ajouter"))
)

// Définir les options pour la sous-commande "remove"
CARGS_OPTIONS(
    remove_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('r', "recursive", HELP("Supprimer récursivement les répertoires")),
    POSITIONAL_STRING("file", HELP("Fichier à supprimer"))
)

// Définir les options principales avec sous-commandes et groupes d'options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Options globales
    OPTION_FLAG('v', "verbose", HELP("Activer la sortie verbeuse")),
    OPTION_STRING('o', "output", HELP("Fichier journal"), 
                 DEFAULT("output.log"), 
                 HINT("FICHIER")),
    
    // Options dans un groupe
    GROUP_START("Display", GROUP_DESC("Options d'affichage")),
        OPTION_FLAG('q', "quiet", HELP("Supprimer la sortie"), 
                   CONFLICTS("verbose")),
        OPTION_FLAG('c', "color", HELP("Coloriser la sortie")),
    GROUP_END(),
    
    // Options dans un groupe exclusif (une seule peut être sélectionnée)
    GROUP_START("Format", GROUP_DESC("Format de sortie"), 
                FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('j', "json", HELP("Sortie JSON")),
        OPTION_FLAG('x', "xml", HELP("Sortie XML")),
        OPTION_FLAG('y', "yaml", HELP("Sortie YAML")),
    GROUP_END(),
    
    // Sous-commandes
    SUBCOMMAND("add", add_options, 
               HELP("Ajouter des fichiers à l'index"), 
               ACTION(add_command)),
    
    SUBCOMMAND("rm", remove_options, 
               HELP("Supprimer des fichiers de l'index"), 
               ACTION(remove_command))
)
```

## Documentation associée

- [Référence des fonctions](functions.md) - Référence complète des fonctions de l'API
- [Référence des types](types.md) - Informations détaillées sur les types de données
- [Motifs Regex](regex_patterns.md) - Motifs d'expressions régulières prédéfinis
