# Validateurs personnalisés

Les validateurs personnalisés étendent les capacités de cargs en vous permettant d'implémenter une logique de validation spécialisée pour les options de ligne de commande.

!!! abstract "Aperçu"
    Ce guide couvre les techniques de validation avancées avec des validateurs personnalisés :
    
    - **Validateurs** et **Pré-validateurs** - Deux types de fonctions de validation personnalisées
    - **Validation pilotée par les données** - Transmission de configuration aux validateurs
    - **Macros d'aide** - Création de composants de validation réutilisables
    - **Bonnes pratiques** - Directives pour une implémentation efficace des validateurs
    
    Pour les concepts de validation de base, consultez le [guide de validation](../guide/validation.md).

## Types de validateurs

cargs prend en charge deux types de fonctions de validation personnalisées :

### 1. Validateurs

Les validateurs vérifient la valeur **traitée** après que le gestionnaire l'a convertie en son type final :

```c
int validator_function(cargs_t *cargs, cargs_value_t value, validator_data_t data);
```

Utilisez les validateurs lorsque vous devez valider en fonction du type de la valeur traitée (int, float, etc.).

### 2. Pré-validateurs

Les pré-validateurs vérifient la **chaîne brute** avant qu'elle ne soit traitée par le gestionnaire :

```c
int pre_validator_function(cargs_t *cargs, const char *value, validator_data_t data);
```

Utilisez les pré-validateurs lorsque vous devez :
- Examiner le format de la chaîne brute avant l'analyse
- Effectuer une validation complexe de chaîne
- Valider avant de tenter la conversion

## Création de validateurs personnalisés

### Validateur de base

Créons un validateur simple qui garantit qu'un entier est pair :

```c
int even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    UNUSED(data);  // Pas d'utilisation de données personnalisées dans cet exemple
    
    if (value.as_int % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                         "La valeur doit être un nombre pair, obtenu %d", value.as_int);
    }
    return CARGS_SUCCESS;
}
```

Pour utiliser ce validateur :

```c
OPTION_INT('n', "number", HELP("Un nombre pair"), 
          VALIDATOR(even_validator, NULL))
```

### Pré-validateur de base

Voici un pré-validateur qui vérifie la longueur de la chaîne avant le traitement :

```c
int string_length_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
    // Obtenir la longueur minimale à partir des données du validateur
    size_t min_length = *(size_t *)data.custom;
    
    if (strlen(value) < min_length) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "La chaîne doit comporter au moins %zu caractères", min_length);
    }
    return CARGS_SUCCESS;
}
```

Pour utiliser ce pré-validateur :

```c
// Définir la contrainte de validation
size_t min_length = 8;

OPTION_STRING('p', "password", HELP("Mot de passe"), 
            PRE_VALIDATOR(string_length_pre_validator, &min_length))
```

## Transmission de données aux validateurs

Le paramètre `validator_data_t` vous permet de transmettre des données personnalisées à vos validateurs pour une validation plus flexible.

### Utilisation de structures de données personnalisées

Créez une structure pour contenir les paramètres de validation :

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_odd;
} number_constraints_t;
```

Créez un validateur qui utilise cette structure :

```c
int custom_number_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Obtenir les contraintes des données du validateur
    number_constraints_t *constraints = (number_constraints_t *)data.custom;
    
    // Valider la plage
    if (value.as_int < constraints->min_value || value.as_int > constraints->max_value) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_RANGE,
                          "La valeur doit être comprise entre %d et %d", 
                          constraints->min_value, constraints->max_value);
    }
    
    // Valider pair/impair si nécessaire
    if (!constraints->allow_odd && (value.as_int % 2 != 0)) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "La valeur doit être un nombre pair");
    }
    
    return CARGS_SUCCESS;
}
```

Utilisez le validateur avec des données personnalisées :

```c
// Définir les contraintes
static number_constraints_t constraints = {
    .min_value = 10,
    .max_value = 100,
    .allow_odd = false
};

OPTION_INT('n', "number", HELP("Un nombre avec contraintes"), 
          VALIDATOR(custom_number_validator, &constraints))
```

### Utilisation de configuration statique

Pour des configurations simples, vous pouvez utiliser des variables statiques :

```c
// Configuration statique pour le validateur de longueur de chaîne
static size_t USERNAME_MIN_LENGTH = 3;
static size_t USERNAME_MAX_LENGTH = 20;

int username_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    UNUSED(data);
    const char *username = value.as_string;
    
    if (username == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Le nom d'utilisateur ne peut pas être NULL");
    }
    
    size_t length = strlen(username);
    if (length < USERNAME_MIN_LENGTH || length > USERNAME_MAX_LENGTH) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Le nom d'utilisateur doit comporter entre %zu et %zu caractères",
                          USERNAME_MIN_LENGTH, USERNAME_MAX_LENGTH);
    }
    
    return CARGS_SUCCESS;
}
```

## Techniques de validation avancées

### Variables inline avec littéraux composés

Vous pouvez transmettre des données sans variable séparée en utilisant des littéraux composés :

```c
OPTION_STRING('u', "username", HELP("Nom d'utilisateur"),
             PRE_VALIDATOR(string_length_pre_validator, &((size_t){3})))
```

Cela crée une variable anonyme `size_t` avec la valeur 3 et transmet son adresse au validateur.

### Validation consciente du contexte

Parfois, les validateurs doivent vérifier les valeurs par rapport à d'autres options :

```c
typedef struct {
    const char *related_option;
} option_relation_t;

int greater_than_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    option_relation_t *relation = (option_relation_t *)data.custom;
    cargs_value_t other_value = cargs_get(*cargs, relation->related_option);
    
    if (value.as_int <= other_value.as_int) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "La valeur doit être supérieure à '%s' (%d)",
                          relation->related_option, other_value.as_int);
    }
    
    return CARGS_SUCCESS;
}

// Utilisation
static option_relation_t max_relation = { .related_option = "min" };

CARGS_OPTIONS(
    options,
    OPTION_INT('n', "min", HELP("Valeur minimale")),
    OPTION_INT('x', "max", HELP("Valeur maximale"), 
               VALIDATOR(greater_than_validator, &max_relation))
)
```

### Création de macros d'aide

Pour les modèles de validation fréquemment utilisés, créez des macros d'aide :

```c
// Macro d'aide pour la validation des nombres pairs
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)

// Macro d'aide pour la longueur minimale de chaîne
#define MIN_LENGTH(min) \
    PRE_VALIDATOR(string_length_pre_validator, &((size_t){min}))

// Macro d'aide pour la longueur maximale de chaîne
#define MAX_LENGTH(max) \
    PRE_VALIDATOR(string_length_max_validator, &((size_t){max}))

// Vérification de longueur combinée
#define STRING_LENGTH(min, max) \
    PRE_VALIDATOR(string_length_range_validator, &((length_range_t){min, max}))

// Utilisation
CARGS_OPTIONS(
    options,
    OPTION_INT('n', "number", HELP("Un nombre pair"), EVEN_NUMBER()),
    OPTION_STRING('p', "password", HELP("Mot de passe"), MIN_LENGTH(8)),
    OPTION_STRING('u', "username", HELP("Nom d'utilisateur"), STRING_LENGTH(3, 20))
)
```

## Rapport d'erreurs

Les validateurs doivent utiliser `CARGS_REPORT_ERROR` pour signaler les échecs de validation :

```c
CARGS_REPORT_ERROR(cargs, error_code, format_string, ...);
```

Codes d'erreur courants :

| Code d'erreur | Description | Utilisation typique |
|------------|-------------|-------------|
| `CARGS_ERROR_INVALID_VALUE` | La valeur ne répond pas aux exigences | Échecs de validation généraux |
| `CARGS_ERROR_INVALID_RANGE` | Valeur hors de la plage autorisée | Validation de plage |
| `CARGS_ERROR_INVALID_FORMAT` | La valeur a un format incorrect | Validation de format |
| `CARGS_ERROR_MEMORY` | Échec de l'allocation mémoire | Pendant le traitement de la validation |

## Bonnes pratiques

### 1. Responsabilité unique

Chaque validateur doit se concentrer sur une préoccupation de validation :

=== "Bon : Validateurs ciblés"
    ```c
    int is_even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data);
    int in_range_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data);
    
    // Utiliser les deux validateurs ensemble
    OPTION_INT('n', "number", "Nombre", 
              VALIDATOR(is_even_validator, NULL),
              VALIDATOR(in_range_validator, &range))
    ```

=== "Mauvais : Validateur monolithique"
    ```c
    int complex_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
    {
        // Fait trop de choses dans une seule fonction
        // - Vérifie si la valeur est paire
        // - Valide la plage
        // - Vérifie les nombres premiers
        // - Vérifie les règles métier spéciales
        // ...
    }
    ```

### 2. Messages d'erreur descriptifs

Fournissez des messages d'erreur clairs et exploitables :

=== "Bon : Messages utiles"
    ```c
    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                      "Le nom d'utilisateur doit comporter 3 à 20 caractères et contenir uniquement des lettres, des chiffres et des underscores");
    ```

=== "Mauvais : Messages peu clairs"
    ```c
    CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "Entrée invalide");
    ```

### 3. Sécurité des paramètres

Validez toujours les paramètres et gérez les cas limites :

```c
int string_length_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Vérifier si la valeur est NULL
    if (value.as_string == NULL) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE, "La chaîne ne peut pas être NULL");
    }
    
    // Reste de la logique de validation...
    return CARGS_SUCCESS;
}
```

### 4. Considérations de mémoire

Évitez les allocations excessives dans les validateurs :

```c
int efficient_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    // Utiliser des tampons basés sur la pile pour les opérations temporaires
    char buffer[256];
    
    // Traiter la valeur sans allocations inutiles sur le tas
    
    return CARGS_SUCCESS;
}
```

### 5. Composants réutilisables

Concevez des validateurs pour être réutilisables sur plusieurs options :

```c
// Validateur générique pour vérifier si un nombre est divisible par n
int divisible_by_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    int divisor = *(int *)data.custom;
    
    if (value.as_int % divisor != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "La valeur doit être divisible par %d", divisor);
    }
    
    return CARGS_SUCCESS;
}

// Réutilisable sur différentes options
OPTION_INT('n', "number", "Nombre divisible par 2", 
          VALIDATOR(divisible_by_validator, &((int){2})));

OPTION_INT('m', "multiple", "Multiple de 5", 
          VALIDATOR(divisible_by_validator, &((int){5})));
```

## Exemple complet

Voici un exemple complet démontrant diverses techniques de validateurs personnalisés :

```c
#include "cargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Validateur personnalisé pour les adresses email
int email_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    (void)data; // Paramètre non utilisé
    
    const char* email = value.as_string;
    if (!email) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "L'adresse email ne peut pas être NULL");
    }
    
    // Vérifier le caractère @
    const char* at = strchr(email, '@');
    if (!at) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "L'adresse email doit contenir un caractère '@'");
    }
    
    // Vérifier le domaine
    const char* dot = strchr(at, '.');
    if (!dot) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "Le domaine de l'email doit contenir un caractère '.'");
    }
    
    return CARGS_SUCCESS;
}

// Validateur personnalisé pour les nombres pairs
int even_validator(cargs_t *cargs, cargs_value_t value, validator_data_t data)
{
    (void)data; // Paramètre non utilisé
    
    int number = value.as_int;
    if (number % 2 != 0) {
        CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                          "La valeur doit être un nombre pair");
    }
    
    return CARGS_SUCCESS;
}

// Pré-validateur personnalisé pour les exigences de casse de chaîne
int case_pre_validator(cargs_t *cargs, const char *value, validator_data_t data)
{
    typedef enum { LOWERCASE, UPPERCASE, MIXED } case_requirement_t;
    case_requirement_t req = *(case_requirement_t *)data.custom;
    
    bool has_upper = false;
    bool has_lower = false;
    
    for (const char *p = value; *p; p++) {
        if (isupper(*p)) has_upper = true;
        if (islower(*p)) has_lower = true;
    }
    
    switch (req) {
        case LOWERCASE:
            if (has_upper) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "La valeur doit être en minuscules uniquement");
            }
            break;
            
        case UPPERCASE:
            if (has_lower) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "La valeur doit être en majuscules uniquement");
            }
            break;
            
        case MIXED:
            if (!has_upper || !has_lower) {
                CARGS_REPORT_ERROR(cargs, CARGS_ERROR_INVALID_VALUE,
                                 "La valeur doit contenir à la fois des lettres majuscules et minuscules");
            }
            break;
    }
    
    return CARGS_SUCCESS;
}

// Macros d'aide pour les validations courantes
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)
#define EMAIL_VALIDATOR() VALIDATOR(email_validator, NULL)
#define LOWERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){LOWERCASE}))
#define UPPERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){UPPERCASE}))
#define MIXED_CASE() PRE_VALIDATOR(case_pre_validator, &((int){MIXED}))

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Validateur de plage intégré
    OPTION_INT('p', "port", "Numéro de port", 
                DEFAULT(8080), RANGE(1, 65535)),
    
    // Validateur de choix intégré
    OPTION_STRING('l', "log-level", "Niveau de journalisation", 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Validateur d'email personnalisé
    OPTION_STRING('e', "email", "Adresse email",
                EMAIL_VALIDATOR()),
    
    // Validateur de nombre pair personnalisé
    OPTION_INT('n', "number", "Un nombre pair",
                EVEN_NUMBER(),
                DEFAULT(42)),
    
    // Chaîne avec validation de casse
    OPTION_STRING('u', "username", "Nom d'utilisateur (minuscules)",
                LOWERCASE_ONLY()),
                
    // Chaîne avec plusieurs validateurs
    OPTION_STRING('p', "password", "Mot de passe (casse mixte)",
                MIXED_CASE())
)

int main(int argc, char **argv) {
    cargs_t cargs = cargs_init(options, "validators_example", "1.0.0");
    cargs.description = "Exemple de validateurs personnalisés";
    
    int status = cargs_parse(&cargs, argc, argv);
    if (status != CARGS_SUCCESS) {
        return status;
    }
    
    // Accéder aux valeurs analysées
    int port = cargs_get(cargs, "port").as_int;
    const char* log_level = cargs_get(cargs, "log-level").as_string;
    int number = cargs_get(cargs, "number").as_int;
    
    const char* email = cargs_is_set(cargs, "email") ? 
                        cargs_get(cargs, "email").as_string : "non défini";
    
    const char* username = cargs_is_set(cargs, "username") ?
                          cargs_get(cargs, "username").as_string : "non défini";
    
    const char* password = cargs_is_set(cargs, "password") ?
                          cargs_get(cargs, "password").as_string : "non défini";
    
    printf("Valeurs validées :\n");
    printf("  Port : %d (plage : 1-65535)\n", port);
    printf("  Niveau de journalisation : %s (choix : debug, info, warning, error)\n", log_level);
    printf("  Nombre pair : %d (doit être pair)\n", number);
    printf("  Email : %s (doit être un format d'email valide)\n", email);
    printf("  Nom d'utilisateur : %s (doit être en minuscules)\n", username);
    printf("  Mot de passe : %s (doit contenir une casse mixte)\n", password);
    
    cargs_free(&cargs);
    return 0;
}
```

## Documentation connexe

- [Guide de validation](../guide/validation.md) - Concepts de validation de base
- [Guide des expressions régulières](regex.md) - Validation avec des motifs regex
