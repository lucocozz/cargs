# Référence des types

Cette page documente les types de données et structures de base fournis par la bibliothèque cargs. La compréhension de ces types est essentielle pour une utilisation avancée et des extensions personnalisées.

!!! abstract "Aperçu"
    cargs définit plusieurs types de données et structures clés qui forment la base de la bibliothèque :
    
    - **Structures de base** - Structures de données principales comme `cargs_t` et `cargs_option_t`
    - **Types de valeurs** - Types pour stocker et accéder à différentes sortes de valeurs
    - **Énumérations** - Classificateurs de types et définitions de drapeaux
    - **Collections** - Structures de données de tableaux et de maps, et itérateurs
    - **Types de callbacks** - Types de pointeurs de fonction pour les gestionnaires et validateurs

## Structures de base

### cargs_t

La structure de contexte principale qui contient tous les états d'analyse et la configuration.

```c
typedef struct cargs_s {
    /* Champs publics */
    const char *program_name;    // Nom du programme
    const char *version;         // Version du programme
    const char *description;     // Description du programme
    const char *env_prefix;      // Préfixe pour les variables d'environnement
    
    /* Champs internes - ne pas accéder directement */
    cargs_option_t     *options;      // Options définies
    cargs_error_stack_t error_stack;  // Pile d'erreurs
    struct { /* ... */ } context;     // Contexte interne
} cargs_t;
```

Les champs publics peuvent être accédés et modifiés directement après l'initialisation avec `cargs_init()` :

```c
cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
cargs.description = "Mon super programme";
cargs.env_prefix = "MYAPP";  // Optionnel : préfixe pour les variables d'environnement
```

!!! warning "Champs internes"
    Les champs internes ne doivent pas être accédés directement. Utilisez les fonctions API fournies pour interagir avec eux.

### cargs_option_t

Définit une option de ligne de commande avec toutes ses propriétés et son comportement.

```c
typedef struct cargs_option_s {
    /* Métadonnées de base */
    cargs_optype_t type;        // Type d'option (drapeau, positionnel, etc.)
    const char *name;          // Nom interne pour les références
    char sname;                // Nom court (ex., 'v' pour -v)
    const char *lname;         // Nom long (ex., "verbose" pour --verbose)
    const char *help;          // Texte d'aide
    const char *hint;          // Indice de valeur pour l'affichage d'aide
    
    /* Métadonnées de valeur */
    cargs_valtype_t value_type;   // Type de valeur
    cargs_value_t value;             // Valeur actuelle
    cargs_value_t default_value;     // Valeur par défaut
    bool have_default;         // Si une valeur par défaut est définie
    /* Champs de valeur supplémentaires... */
    
    /* Callbacks */
    cargs_handler_t handler;       // Fonction gestionnaire de valeur
    cargs_free_handler_t free_handler;  // Fonction de nettoyage des ressources
    cargs_validator_t validator;    // Validateur de valeur
    /* Champs de callback supplémentaires... */
    
    /* Dépendances */
    const char **requires;     // Options qui doivent être définies avec celle-ci
    const char **conflicts;    // Options qui ne peuvent pas être utilisées avec celle-ci
    
    /* État */
    cargs_optflags_t flags;      // Drapeaux de comportement d'option
    bool is_set;               // Si l'option a été définie en ligne de commande
    
    /* Champs de sous-commande */
    cargs_action_t action;     // Action pour les sous-commandes
    struct cargs_option_s *sub_options;  // Options pour les sous-commandes
} cargs_option_t;
```

!!! note
    Vous ne créez généralement pas de structures `cargs_option_t` directement. Utilisez plutôt les macros fournies comme `OPTION_STRING`, `OPTION_INT`, etc.

## Types de valeurs

### cargs_value_t

Un type union qui peut contenir des valeurs de différents types :

```c
typedef union cargs_value_u {
    uintptr_t raw;          // Valeur brute en entier
    void     *as_ptr;       // Pointeur générique
    
    // Types de base
    char  as_char;          // Caractère
    char *as_string;        // Chaîne
    int   as_int;           // Entier
    long long as_int64;     // Entier 64 bits
    double as_float;        // Flottant
    bool   as_bool;         // Booléen
    
    // Types de collection
    cargs_value_t      *as_array;  // Tableau de valeurs
    cargs_pair_t *as_map;    // Map clé-valeur
} cargs_value_t;
```

### cargs_pair_t

Une paire clé-valeur utilisée dans les collections de type map :

```c
typedef struct cargs_pair_s {
    const char *key;    // Clé chaîne
    cargs_value_t     value;  // Valeur de n'importe quel type supporté
} cargs_pair_t;
```

## Énumérations

### cargs_optype_t

Définit les différents types d'éléments de ligne de commande :

```c
typedef enum cargs_optype_e {
    TYPE_NONE = 0,        // Terminateur pour les tableaux d'options
    TYPE_OPTION,          // Option standard avec préfixe - ou --
    TYPE_GROUP,           // Groupement logique d'options
    TYPE_POSITIONAL,      // Argument positionnel
    TYPE_SUBCOMMAND,      // Sous-commande avec ses propres options
} cargs_optype_t;
```

### cargs_valtype_t

Définit les types de valeurs qu'une option peut contenir :

```c
typedef enum cargs_valtype_e {
    VALUE_TYPE_NONE = 0,        // Pas de valeur
    
    // Types primitifs
    VALUE_TYPE_INT    = 1 << 0,  // Entier
    VALUE_TYPE_STRING = 1 << 1,  // Chaîne
    VALUE_TYPE_FLOAT  = 1 << 2,  // Flottant
    VALUE_TYPE_BOOL   = 1 << 3,  // Booléen
    VALUE_TYPE_FLAG   = 1 << 4,  // Drapeau (booléen sans valeur)
    
    // Types de tableau
    VALUE_TYPE_ARRAY_STRING = 1 << 5,  // Tableau de chaînes
    VALUE_TYPE_ARRAY_INT    = 1 << 6,  // Tableau d'entiers
    VALUE_TYPE_ARRAY_FLOAT  = 1 << 7,  // Tableau de flottants
    
    // Types de map
    VALUE_TYPE_MAP_STRING = 1 << 8,    // Map de chaînes
    VALUE_TYPE_MAP_INT    = 1 << 9,    // Map d'entiers
    VALUE_TYPE_MAP_FLOAT  = 1 << 10,    // Map de flottants
    VALUE_TYPE_MAP_BOOL   = 1 << 11,   // Map de booléens
    
    VALUE_TYPE_CUSTOM = 1 << 12,       // Type personnalisé
} cargs_valtype_t;
```

### cargs_optflags_t

Définit des drapeaux qui modifient le comportement des options :

```c
typedef enum cargs_optflags_e {
    FLAG_NONE = 0,
    /* Drapeaux d'option */
    FLAG_REQUIRED      = 1 << 0,  // L'option doit être spécifiée
    FLAG_HIDDEN        = 1 << 1,  // L'option est masquée dans l'aide
    FLAG_ADVANCED      = 1 << 2,  // L'option est marquée comme avancée
    FLAG_DEPRECATED    = 1 << 3,  // L'option est marquée comme obsolète
    FLAG_EXPERIMENTAL  = 1 << 4,  // L'option est marquée comme expérimentale
    FLAG_EXIT          = 1 << 5,  // Le programme se termine après le traitement de l'option
    FLAG_ENV_OVERRIDE  = 1 << 6,  // La valeur de l'option peut être remplacée par une variable d'environnement
    FLAG_AUTO_ENV      = 1 << 7,  // Générer automatiquement le nom de variable d'environnement
    FLAG_NO_ENV_PREFIX = 1 << 8,  // Ne pas utiliser le préfixe de variable d'environnement
    
    /* Drapeaux de type tableau et map */
    FLAG_SORTED       = 1 << 9,   // Les valeurs du tableau sont triées
    FLAG_UNIQUE       = 1 << 10,  // Les valeurs du tableau sont uniques
    FLAG_SORTED_VALUE = 1 << 11,  // Les valeurs de la map sont triées
    FLAG_SORTED_KEY   = 1 << 12,  // Les clés de la map sont triées
    FLAG_UNIQUE_VALUE = 1 << 13,  // Les valeurs de la map sont uniques
    
    /* Drapeaux de groupe */
    FLAG_EXCLUSIVE = 1 << 14,     // Une seule option du groupe peut être définie
} cargs_optflags_t;
```

## Itérateurs de collections

### cargs_array_it_t

Itérateur pour les collections de type tableau :

```c
typedef struct cargs_array_iterator_s {
    cargs_value_t *_array;      // Pointeur interne du tableau
    size_t   _count;      // Nombre d'éléments
    size_t   _position;   // Position actuelle
    cargs_value_t  value;       // Valeur actuelle
} cargs_array_it_t;
```

Exemple d'utilisation :
```c
cargs_array_it_t it = cargs_array_it(cargs, "names");
while (cargs_array_next(&it)) {
    printf("Nom : %s\n", it.value.as_string);
}
```

### cargs_map_it_t

Itérateur pour les collections de type map :

```c
typedef struct cargs_map_iterator_s {
    cargs_pair_t *_map;    // Pointeur interne de la map
    size_t        _count;  // Nombre d'éléments
    size_t        _position; // Position actuelle
    const char   *key;     // Clé actuelle
    cargs_value_t       value;   // Valeur actuelle
} cargs_map_it_t;
```

Exemple d'utilisation :
```c
cargs_map_it_t it = cargs_map_it(cargs, "env");
while (cargs_map_next(&it)) {
    printf("%s = %s\n", it.key, it.value.as_string);
}
```

## Types de callbacks

### cargs_handler_t

Fonction gestionnaire pour traiter les valeurs d'options :

```c
typedef int (*cargs_handler_t)(cargs_t *, cargs_option_t *, char *);
```

Les gestionnaires personnalisés traitent la valeur de chaîne brute et la stockent dans la structure d'option.

### cargs_free_handler_t

Fonction gestionnaire pour libérer les ressources des options :

```c
typedef int (*cargs_free_handler_t)(cargs_option_t *);
```

Les gestionnaires de libération personnalisés nettoient les ressources allouées par une fonction gestionnaire.

### cargs_validator_t

Fonction validateur pour vérifier les valeurs d'options :

```c
typedef int (*cargs_validator_t)(cargs_t *, cargs_value_t, validator_data_t);
```

Les validateurs s'assurent que les valeurs répondent à certains critères après traitement.

### cargs_pre_validator_t

Fonction pré-validateur pour vérifier les valeurs de chaînes brutes :

```c
typedef int (*cargs_pre_validator_t)(cargs_t *, const char *, validator_data_t);
```

Les pré-validateurs vérifient les chaînes d'entrée avant qu'elles ne soient traitées.

### cargs_action_t

Fonction d'action pour les sous-commandes :

```c
typedef int (*cargs_action_t)(cargs_t *, void *);
```

Les actions sont exécutées lorsqu'une sous-commande est invoquée.

## Types de données de validateur

### validator_data_t

Données passées aux validateurs :

```c
typedef union validator_data_u {
    void        *custom;  // Données de validateur personnalisé
    range_t      range;   // Limites de plage pour la validation numérique
    regex_data_t regex;   // Motif regex et informations
} validator_data_t;
```

### range_t

Limites de plage pour la validation numérique :

```c
typedef struct range_s {
    long long min;  // Valeur minimum
    long long max;  // Valeur maximum
} range_t;
```

### regex_data_t

Données de validation par expression régulière :

```c
typedef struct regex_data_s {
    const char *pattern;  // Chaîne du motif regex
    const char *hint;     // Indice de message d'erreur
} regex_data_t;
```

## Types de gestion d'erreurs

### cargs_error_type_t

Codes d'erreur retournés par les fonctions cargs :

```c
typedef enum cargs_error_type_e {
    CARGS_SUCCESS = 0,             // Pas d'erreur
    CARGS_SOULD_EXIT,              // Sortie normale après traitement d'option
    
    /* Erreurs de structure */
    CARGS_ERROR_DUPLICATE_OPTION,  // Collision de nom d'option
    CARGS_ERROR_INVALID_HANDLER,   // Fonction gestionnaire invalide
    /* Plus de codes d'erreur... */
    
    /* Erreurs d'analyse */
    CARGS_ERROR_INVALID_ARGUMENT,  // Argument invalide
    CARGS_ERROR_MISSING_VALUE,     // Valeur requise non fournie
    /* Plus de codes d'erreur... */
} cargs_error_type_t;
```

### cargs_error_t

Structure pour stocker des informations d'erreur détaillées :

```c
typedef struct cargs_error_s {
    cargs_error_context_t context;  // Contexte d'erreur
    int code;                       // Code d'erreur
    char message[CARGS_MAX_ERROR_MESSAGE_SIZE];  // Message d'erreur
} cargs_error_t;
```

## Constantes et limites

Cargs définit plusieurs constantes qui contrôlent son comportement :

```c
#define MAX_SUBCOMMAND_DEPTH 8  // Profondeur maximale des sous-commandes imbriquées
#define CARGS_MAX_ERROR_MESSAGE_SIZE 256  // Longueur maximale des messages d'erreur
#define CARGS_MAX_ERRORS_STACK 16  // Nombre maximum d'erreurs dans la pile
#define MULTI_VALUE_INITIAL_CAPACITY 8  // Capacité initiale pour les collections
```

## Macros et catégories de types

Cargs fournit plusieurs macros d'aide pour travailler avec les types de valeurs :

```c
// Macros de catégorie de type
#define VALUE_TYPE_PRIMITIVE \
    (VALUE_TYPE_INT | VALUE_TYPE_STRING | VALUE_TYPE_FLOAT | VALUE_TYPE_BOOL)
    
#define VALUE_TYPE_ARRAY \
    (VALUE_TYPE_ARRAY_STRING | VALUE_TYPE_ARRAY_INT | VALUE_TYPE_ARRAY_FLOAT)
    
#define VALUE_TYPE_MAP \
    (VALUE_TYPE_MAP_STRING | VALUE_TYPE_MAP_INT | VALUE_TYPE_MAP_FLOAT | VALUE_TYPE_MAP_BOOL)

// Macros de catégorie de drapeau
#define OPTION_FLAG_MASK \
    (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_EXIT | VERSIONING_FLAG_MASK)
    
#define OPTION_ARRAY_FLAG_MASK \
    (FLAG_SORTED | FLAG_UNIQUE | VERSIONING_FLAG_MASK)
    
// Plus de masques de drapeaux...
```

## Types utilitaires

La bibliothèque inclut également plusieurs types utilitaires pour le suivi des erreurs et la gestion du contexte :

```c
typedef struct cargs_error_context_s {
    const char *option_name;     // Nom d'option actuel
    const char *group_name;      // Nom de groupe actuel 
    const char *subcommand_name; // Nom de sous-commande actuel
} cargs_error_context_t;

typedef struct cargs_error_stack_s {
    cargs_error_t errors[CARGS_MAX_ERRORS_STACK];  // Tableau d'erreurs
    size_t count;                                  // Nombre d'erreurs
} cargs_error_stack_t;
```

## Documentation associée

- [Référence des macros](macros.md) - Liste complète des macros de définition d'options
- [Référence des fonctions](functions.md) - Référence complète des fonctions API
- [Aperçu](overview.md) - Vue d'ensemble de haut niveau de l'API cargs
