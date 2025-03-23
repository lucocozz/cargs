# Installation

This page explains how to install the cargs library in different environments.

## Prerequisites

cargs has a single external dependency:

- **PCRE2**: Required for regular expression validation support
  - Ubuntu/Debian: `apt install libpcre2-dev`
  - Fedora/CentOS: `dnf install pcre2-devel`
  - macOS: `brew install pcre2`

## Installation from Source

### With Meson

```bash
# Clone the repository
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Build
meson setup builddir
meson compile -C builddir

# Installation (requires permissions)
meson install -C builddir
```

### With Just (recommended for development)

```bash
# Clone the repository
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Install Just if you don't have it
# https://github.com/casey/just

# Build static and shared libraries
just build

# Libraries are now available:
# - libcargs.so (shared library)
# - libcargs.a (static library)

# Installation
just install
```

## Using as a Static Library

If you prefer not to install the library system-wide, you can:

1. Build the project as indicated above
2. Copy `libcargs.a` into your project
3. Copy the `includes/` directory into your project
4. Link with the static library:
   ```bash
   gcc your_program.c -o your_program -L/path/to/libcargs.a -lcargs -lpcre2-8
   ```

## As a Dependency in a Meson Project

```meson
cargs_dep = dependency('cargs', fallback: ['cargs', 'cargs_dep'])
```

## Packaging

> **Note:** Official packages for various distributions are planned for future releases.
