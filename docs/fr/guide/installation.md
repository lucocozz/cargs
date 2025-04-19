# Guide d'Installation

Ce guide explique comment installer la bibliothèque cargs dans différents environnements.

## Référence Rapide

| Méthode | Commande | Idéal Pour |
|--------|---------|----------|
| **Gestionnaires de Paquets** | `conan install libcargs/1.0.1@` | Utilisation en production |
| **Depuis les Sources** | `meson setup .build && meson compile -C .build` | Développement |
| **Avec Just** | `just build` | Flux de développement |

## Prérequis

cargs n'a qu'une seule dépendance optionnelle :

!!! info "Dépendance PCRE2"
    **PCRE2** : Requis uniquement pour la validation par expressions régulières.
    
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

## Méthodes d'Installation

### Gestionnaires de Paquets (Recommandé)

#### Conan

```bash
# Installation depuis Conan Center
conan install libcargs/1.0.1@

# Avec des options spécifiques
conan install libcargs/1.0.1@ -o libcargs:disable_regex=true
```

Dans votre fichier `conanfile.txt` :
```
[requires]
libcargs/1.0.1

[options]
libcargs:disable_regex=False
```

#### vcpkg

```bash
# Installation depuis le registre vcpkg
vcpkg install libcargs

# Sans support des regex
vcpkg install libcargs[core]
```

Dans votre fichier `vcpkg.json` :
```json
{
  "dependencies": [
    {
      "name": "libcargs",
      "features": ["regex"]
    }
  ]
}
```

### Compilation Depuis les Sources

#### Utilisation de Meson

```bash
# Cloner le dépôt
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Compiler
meson setup .build
meson compile -C .build

# Installer (nécessite des permissions)
meson install -C .build
```

#### Utilisation de Just (Flux de Développement)

!!! tip "Just"
    [Just](https://github.com/casey/just) est un exécuteur de commandes qui simplifie les tâches courantes.

```bash
# Cloner le dépôt
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Compiler les bibliothèques statiques et partagées
just build

# Installer
just install
```

### Utilisation comme Bibliothèque Statique

Si vous préférez ne pas installer au niveau système :

1. Compilez le projet en utilisant n'importe quelle méthode ci-dessus
2. Copiez `libcargs.a` dans votre projet
3. Copiez le répertoire `includes/` dans votre projet
4. Liez avec la bibliothèque statique :

```bash
gcc votre_programme.c -o votre_programme -L/chemin/vers/libcargs.a -lcargs
```

### Comme Dépendance Meson

```meson
# Dans votre meson.build
cargs_dep = dependency('cargs', version: '>=1.0.0', required: false)

# Repli sur un sous-projet si non trouvé au niveau système
if not cargs_dep.found()
  cargs_proj = subproject('cargs')
  cargs_dep = cargs_proj.get_variable('cargs_dep')
endif
```

## Options de Configuration

### Désactiver le Support des Regex

Si vous n'avez pas besoin de validation par regex, vous pouvez compiler sans la dépendance PCRE2 :

=== "Meson"
    ```bash
    meson setup -Ddisable_regex=true .build
    ```

=== "Just"
    ```bash
    just disable_regex=true build
    ```

=== "Conan"
    ```bash
    conan install . -o libcargs:disable_regex=true
    ```

=== "vcpkg"
    ```bash
    vcpkg install libcargs --features=""
    ```

Quand le support des regex est désactivé :
- Aucune dépendance PCRE2 n'est requise
- Le validateur `REGEX()` devient une fonction sans effet
- Tous les motifs prédéfinis dans `cargs/regex.h` sont définis mais ne fonctionneront pas
- La macro `CARGS_NO_REGEX` est définie pour la compilation conditionnelle

### Optimisation des Performances

Pour les déploiements en production, activez le mode release pour ignorer la validation lors de l'initialisation :

=== "Compilation Manuelle"
    ```bash
    gcc votre_programme.c -o votre_programme -DCARGS_RELEASE -lcargs
    ```

=== "Meson"
    ```meson
    add_project_arguments('-DCARGS_RELEASE', language: 'c')
    ```

## Vérification de l'Installation

=== "Vérifier les Fichiers"
    ```bash
    # Vérifier la bibliothèque partagée
    ls -la /usr/local/lib/libcargs.so*
    
    # Vérifier les en-têtes
    ls -la /usr/local/include/cargs*
    ```

=== "Test de Compilation"
    ```bash
    # Compiler un programme exemple
    gcc -o test_cargs test.c -lcargs
    
    # Exécuter le programme
    ./test_cargs --help
    ```
