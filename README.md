<p align="center">
  <img src="docs/assets/logo.png" alt="cargs logo" width="200">
</p>

# cargs

![CI/CD Pipeline](https://github.com/lucocozz/cargs/actions/workflows/ci.yml/badge.svg)
![CodeQL Analysis](https://github.com/lucocozz/cargs/actions/workflows/codeql.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

> Modern C library for command-line argument parsing with an elegant, macro-based API.

**cargs** is a powerful C library for handling command-line arguments, designed to be both simple to use and flexible enough for advanced usage scenarios.

## ✨ Features

- 🚀 **Elegant design**: define options with concise, expressive macros
- 🌳 **Subcommands**: Git/Docker-style nested command support
- 🔍 **Built-in validation**: range, regex, custom validators
- 🔄 **Typed options**: booleans, integers, strings, floats, arrays, maps
- 🌐 **Environment variables**: automatic ENV configuration
- 📋 **Help generation**: automatic formatted help and usage display
- ⚡ **High performance**: optimized for speed and low memory footprint

## 📦 Installation

### ⚙️ Dependencies

- **PCRE2**: Required for regex validation support
  - Ubuntu/Debian: `apt install libpcre2-dev`
  - Fedora/CentOS: `dnf install pcre2-devel`
  - macOS: `brew install pcre2`

### 💻 From Source

#### 🛠️ Using Meson

```bash
# Clone the repo
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Build
meson setup builddir
meson compile -C builddir

# Install (requires permissions)
meson install -C builddir
```

#### ⚡ Using Just (recommended for development)

```bash
# Clone the repo
git clone https://github.com/lucocozz/cargs.git
cd cargs

# Install Just if you don't have it
# https://github.com/casey/just

# Build both static and shared libraries
just build

# The libraries are now available:
# - libcargs.so (shared library)
# - libcargs.a (static library)

# Install
just install
```

### 📚 Using as a Static Library

If you prefer not to install the library system-wide, you can:

1. Build the project as shown above
2. Copy `libcargs.a` to your project
3. Copy the `includes/` directory to your project
4. Link against the static library:
   ```bash
   gcc your_program.c -o your_program -L/path/to/libcargs.a -lcargs -lpcre2-8
   ```

### 🧩 As a Dependency in a Meson Project

```meson
cargs_dep = dependency('cargs', fallback: ['cargs', 'cargs_dep'])
```

### Packaging

> **Note:** Official packages for various distributions are planned for future releases.


## Quick Example

```c
#include "cargs.h"
#include <stdio.h>

// Define options
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", "Enable verbose output"),
    OPTION_STRING('o', "output", "Output file", DEFAULT("output.txt")),
    OPTION_INT('p', "port", "Port number", RANGE(1, 65535), DEFAULT(8080)),
    POSITIONAL_STRING("input", "Input file")
)

int main(int argc, char **argv)
{
    // Initialize cargs
    cargs_t cargs = cargs_init(options, "my_program", "1.0.0");
    cargs.description = "cargs demonstrator";

    // Parse arguments
    if (cargs_parse(&cargs, argc, argv) != CARGS_SUCCESS) {
        return 1;
    }

    // Access parsed values
    const char *input = cargs_get(cargs, "input").as_string;
    const char *output = cargs_get(cargs, "output").as_string;
    int port = cargs_get(cargs, "port").as_int;
    bool verbose = cargs_get(cargs, "verbose").as_bool;

    printf("Configuration:\n");
    printf("  Input: %s\n", input);
    printf("  Output: %s\n", output);
    printf("  Port: %d\n", port);
    printf("  Verbose: %s\n", verbose ? "yes" : "no");

    // Free resources
    cargs_free(&cargs);
    return 0;
}
```

## ⚖️ Comparison with Alternatives

cargs offers several advantages over existing command-line parsing libraries:

| Feature | cargs | getopt | argp | argtable3 |
|---------|-------|--------|------|-----------|
| Concise macro-based API | ✅ | ❌ | ❌ | ❌ |
| Type Safety | ✅ | ❌ | ❌ | ✅ |
| Nested Subcommands | ✅ | ❌ | ❌ | ❌ |
| Built-in Validation | ✅ | ❌ | ❌ | ✅ |
| Environment Variables | ✅ | ❌ | ❌ | ❌ |
| Maps & Arrays | ✅ | ❌ | ❌ | ❌ |
| Regex Support | ✅ | ❌ | ❌ | ❌ |
| Auto-generated Help | ✅ | ❌ | ✅ | ✅ |
| Dependencies | PCRE2 | libc | GNU | None |
| Learning Curve | Low | Medium | High | Medium |

While getopt provides a minimal POSIX-compliant interface and argp offers more functionality for GNU systems, cargs focuses on providing a modern, type-safe, and elegant approach to command-line parsing. Unlike other libraries that require verbose procedural code, cargs enables concise, maintainable option definitions with its macro-based system.

The key differentiator of cargs is its balance between ease of use and powerful features, making it suitable for both simple utilities and complex command-line applications.

## 🔥 Advanced Features

### Subcommands (Git-style)

```c
CARGS_OPTIONS(
    add_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('f', "force", "Force add operation")
)

CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    SUBCOMMAND("add", add_options, HELP("Add a file"))
)
```

### Regex Validation

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_STRING('e', "email", "Email address", 
                REGEX(CARGS_RE_EMAIL))
)
```

### Multi-value Collections

```c
CARGS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_ARRAY_INT('n', "numbers", "List of numbers", 
                    FLAGS(FLAG_SORTED | FLAG_UNIQUE)),
    OPTION_MAP_STRING('e', "env", "Environment variables")
)

// Usage: --numbers=1,2,3 --env=KEY1=value1,KEY2=value2
```

## 📚 Documentation

For complete documentation, please visit [the online documentation](https://cargs.readthedocs.io/).

Check the `examples/` directory for more practical examples.

## 🗺️ Roadmap

Future planned features:

- 📄 **Configuration files support**: Parse options from JSON, YAML, or other config file formats
- 🪶 **Lightweight version**: Reduced footprint version for constrained systems 
- 🎨 **Themed help output**: Customizable colored help display with theme support
- ✅ **Advanced validators**: 
  - Date/time format validation
  - File existence and type checking
  - String length and array size constraints
  - Semantic versioning validation
  - Contextual validation (based on other option values)

## 👥 Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  <i>Built with ❤️ by <a href="https://github.com/lucocozz">lucocozz</a></i>
</p>
