project(
    'cargs', 'c',
    version: '0.1.0',
    license: 'MIT',
    default_options: [
        'warning_level=3',
        'werror=true',
        'c_std=gnu11',
        'buildtype=debugoptimized',
    ],
    meson_version: '>=1.0.0',
)

# Compiler configuration based on type (Meson ≥ 1.0.0 compatible)
cc = meson.get_compiler('c')
if cc.get_id() == 'clang'
    add_project_arguments('-Wno-gnu-zero-variadic-macro-arguments', language: 'c')
elif cc.get_id() == 'gcc'
    add_project_arguments('-Wno-variadic-macros', language: 'c')
endif

# Include directories
inc_dirs = include_directories('includes')

# Build library
subdir('source')

pcre2_dep = dependency('libpcre2-8', required: true, version: '>=10.00')

# Release mode configuration
if get_option('release_mode')
    add_project_arguments('-DCARGS_RELEASE', language: 'c')
    message('Release mode enabled: validation skipped for better performance')
endif

# Create both static and shared libraries from sources
cargs_lib = both_libraries(
    'cargs',
    cargs_sources,
    include_directories: inc_dirs,
    dependencies: [pcre2_dep],
    version: meson.project_version(),
    soversion: '0',
    install: true,
)

# Create dependency object for internal usage
cargs_dep = declare_dependency(
    link_with: cargs_lib,
    include_directories: inc_dirs,
    dependencies: [pcre2_dep],
)

# Install headers (public API only)
install_headers('includes/cargs.h')
install_subdir(
    'includes/cargs',
    install_dir: get_option('includedir'),
    exclude_directories: ['internal']
)

# Tests if enabled
if get_option('tests')
    subdir('tests')
endif

# Examples if enabled
if get_option('examples')
    subdir('examples')
endif
