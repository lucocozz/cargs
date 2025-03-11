project(
    'cargs', 'c', version: '0.1.0',
    default_options: [
        'warning_level=3',
        'werror=true',
        'c_std=c99',
    ],
)

cc = meson.get_compiler('c')
if cc.get_id() == 'clang'
    add_project_arguments('-Wno-gnu-zero-variadic-macro-arguments', language: 'c')
elif cc.get_id() == 'gcc'
    add_project_arguments('-Wno-variadic-macros', language: 'c')
endif

# Includes complets pour la compilation interne
internal_includes = include_directories('includes')

subdir('source')

cargs_lib = static_library(
    meson.project_name(),
    cargs_sources,
    include_directories: internal_includes,
    install: true,
)

# Pour les utilisateurs de la bibliothèque, exposer seulement les en-têtes publics
public_includes = include_directories('includes')

cargs_dep = declare_dependency(
    link_with: cargs_lib,
    include_directories: public_includes,
)

# Installation des en-têtes publics uniquement
install_headers('includes/cargs.h')

# Installer le répertoire cargs/ en excluant les fichiers et dossiers internes
install_subdir(
    'includes/cargs',
    install_dir: get_option('includedir'),
    exclude_files: [],  # Ajoutez ici les fichiers spécifiques à exclure
    exclude_directories: ['internal']  # Exclut le dossier internal/ 
)

test = get_option('test')
if test
    subdir('tests')
endif
