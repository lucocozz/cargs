# Installation

This page explains how to install the cargs library in different environments.

## Prerequisites

cargs has only one external dependency:

!!! info "PCRE2 Dependency"
    **PCRE2**: Required for regex validation support
    
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

## Installation from source

### With Meson

=== "Clone and build"
    ```bash
    # Clone the repository
    git clone https://github.com/lucocozz/cargs.git
    cd cargs

    # Build
    meson setup .build
    meson compile -C .build
    ```

=== "Installation"
    ```bash
    # Installation (requires permissions)
    meson install -C .build
    ```

### With Just (recommended for development)

!!! tip "Just"
    [Just](https://github.com/casey/just) is a handy command-line tool for running project-specific commands.

=== "Installation and build"
    ```bash
    # Clone the repository
    git clone https://github.com/lucocozz/cargs.git
    cd cargs

    # Build static and shared libraries
    just build
    ```

=== "Result"
    The libraries are now available:
    
    - `libcargs.so` (shared library)
    - `libcargs.a` (static library)

=== "Installation"
    ```bash
    just install
    ```

## Performance Optimization

cargs provides a release mode to optimize performance in production environments.

### Development vs. Release Mode

By default, cargs performs comprehensive validation of your option structures during initialization to catch configuration errors early. This is valuable during development but adds some overhead.

For production deployments, you can enable release mode to skip these validations and improve performance:

=== "Manual Compilation"
    ```bash
    # Add -DCARGS_RELEASE flag to your compilation
    gcc your_program.c -o your_program -DCARGS_RELEASE -lcargs -lpcre2-8
    ```

=== "In a Meson project"
    ```meson
    # In your meson.build
    if get_option('cargs_release')
      add_project_arguments('-DCARGS_RELEASE', language: 'c')
    endif
    ```

!!! tip "When to use Release Mode"
    - **Development**: Keep validation enabled to catch option configuration errors early
    - **Production**: Enable release mode for optimal performance in deployed applications
    - **Testing**: Keep validation enabled to ensure your option structure remains valid

## Using as a static library

If you prefer not to install the library at the system level, you can:

1. Build the project as shown above
2. Copy `libcargs.a` to your project
3. Copy the `includes/` directory to your project
4. Link with the static library:

```bash
gcc your_program.c -o your_program -L/path/to/libcargs.a -lcargs -lpcre2-8
```

## As a dependency in a Meson project

```meson
cargs_dep = dependency('cargs', fallback: ['cargs', 'cargs_dep'])
```

## Alternative installation methods

| Method | Command | Advantages | Disadvantages |
|---------|----------|-----------|---------------|
| **Direct build** | `meson compile -C .build` | Simple, only requires meson/ninja | No system integration |
| **System installation** | `meson install -C .build` | Available for all projects | Requires root permissions |
| **Just** | `just build` | Convenient scripts, simple | Requires just |
| **Local library** | Copy files | Project isolation | Code duplication |

## Packaging

!!! note "Official packages"
    Official packages for various distributions are planned for future releases.

## Verifying installation

After installation, you can verify that everything works correctly:

=== "Check files"
    ```bash
    # Check shared library
    ls -la /usr/local/lib/libcargs.so*
    
    # Check headers
    ls -la /usr/local/include/cargs*
    ```

=== "Compile an example"
    ```bash
    # Compile an example program
    gcc -o test_cargs test.c -lcargs -lpcre2-8
    
    # Run the program
    ./test_cargs --help
    ```
