# Installation

Cette page explique comment installer la bibliothèque cargs dans différents environnements.

## Prérequis

cargs n'a qu'une seule dépendance externe :

!!! info "Dépendance PCRE2"
    **PCRE2** : Requis pour la prise en charge de la validation par expressions régulières (la prise en charge des regex est facultative).
    
    === "Ubuntu/Debian"
        ```bash
        apt install libpcre2-dev
        ```
    
    === "Fedora/CentOS"
        ```bash
        dnf install pcre2-devel
        ```
    
    === "macOS"
        ```bash
        brew install pcre2
        ```

## Installation depuis les sources

### Avec Meson

=== "Cloner et construire"
    ```bash
    # Cloner le dépôt
    git clone https://github.com/lucocozz/cargs.git
    cd cargs

    # Construire
    meson setup .build
    meson compile -C .build
    ```

=== "Installation"
    ```bash
    # Installation (nécessite des permissions)
    meson install -C .build
    ```

### Avec Just (recommandé pour le développement)

!!! tip "Just"
    [Just](https://github.com/casey/just) est un outil de ligne de commande pratique pour exécuter des commandes spécifiques à un projet.

=== "Installation et construction"
    ```bash
    # Cloner le dépôt
    git clone https://github.com/lucocozz/cargs.git
    cd cargs

    # Construire les bibliothèques statiques et partagées
    just build
    ```

=== "Résultat"
    Les bibliothèques sont maintenant disponibles :
    
    - `libcargs.so` (bibliothèque partagée)
    - `libcargs.a` (bibliothèque statique)

=== "Installation"
    ```bash
    just install
    ```

## Construction sans PCRE2 (Désactivation du support des regex)

Si vous n'avez pas besoin de la validation par expressions régulières, vous pouvez construire cargs sans la dépendance PCRE2 :

=== "Avec Meson"
    ```bash
    meson setup -Ddisable_regex=true .build
    meson compile -C .build
    ```

=== "Avec Just"
    ```bash
    just disable_regex=true build
    ```

=== "Avec le script d'installation"
    ```bash
    ./install.sh --disable-regex
    ```

Lorsque le support des regex est désactivé :
- Aucune dépendance PCRE2 n'est requise
- Le validateur `REGEX()` sera un stub qui générera une erreur s'il est utilisé
- Tous les modèles regex prédéfinis dans `cargs/regex.h` seront définis mais non fonctionnels
- La macro `CARGS_NO_REGEX` sera définie, que votre code peut vérifier avec `#ifdef`

## Optimisation des performances

cargs propose un mode release pour optimiser les performances dans les environnements de production.

### Mode développement vs mode release

Par défaut, cargs effectue une validation complète de la structure de vos options lors de l'initialisation pour détecter les erreurs de configuration au plus tôt. Cette fonctionnalité est précieuse pendant le développement mais ajoute une certaine surcharge.

Pour les déploiements en production, vous pouvez activer le mode release pour ignorer ces validations et améliorer les performances :

=== "Compilation manuelle"
    ```bash
    # Ajouter le flag -DCARGS_RELEASE à votre compilation
    gcc votre_programme.c -o votre_programme -DCARGS_RELEASE -lcargs
    ```

=== "Dans un projet Meson"
    ```meson
    # Dans votre fichier meson.build
    if get_option('cargs_release')
      add_project_arguments('-DCARGS_RELEASE', language: 'c')
    endif
    ```

!!! tip "Quand utiliser le mode release"
    - **Développement** : Gardez la validation activée pour détecter rapidement les erreurs de configuration des options
    - **Production** : Activez le mode release pour des performances optimales dans les applications déployées
    - **Tests** : Gardez la validation activée pour vous assurer que la structure de vos options reste valide

## Utilisation comme bibliothèque statique

Si vous préférez ne pas installer la bibliothèque au niveau du système, vous pouvez :

1. Construire le projet comme indiqué ci-dessus
2. Copier `libcargs.a` dans votre projet
3. Copier le répertoire `includes/` dans votre projet
4. Lier avec la bibliothèque statique :

```bash
gcc your_program.c -o your_program -L/path/to/libcargs.a -lcargs
```

## Comme dépendance dans un projet Meson

```meson
cargs_dep = dependency('cargs', fallback: ['cargs', 'cargs_dep'])
```

## Méthodes d'installation alternatives

| Méthode | Commande | Avantages | Inconvénients |
|---------|----------|-----------|---------------|
| **Construction directe** | `meson compile -C .build` | Simple, nécessite seulement meson/ninja | Pas d'intégration système |
| **Installation système** | `meson install -C .build` | Disponible pour tous les projets | Nécessite des droits root |
| **Just** | `just build` | Scripts pratiques, simple | Nécessite just |
| **Bibliothèque locale** | Copier les fichiers | Isolation du projet | Duplication de code |

## Packaging

!!! note "Paquets officiels"
    Des paquets officiels pour diverses distributions sont prévus pour les versions futures.

## Vérification de l'installation

Après l'installation, vous pouvez vérifier que tout fonctionne correctement :

=== "Vérifier les fichiers"
    ```bash
    # Vérifier la bibliothèque partagée
    ls -la /usr/local/lib/libcargs.so*
    
    # Vérifier les en-têtes
    ls -la /usr/local/include/cargs*
    ```

=== "Compiler un exemple"
    ```bash
    # Compiler un programme exemple
    gcc -o test_cargs test.c -lcargs
    
    # Exécuter le programme
    ./test_cargs --help
    ```
