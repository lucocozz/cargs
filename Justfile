name := "cargs"
build_dir := ".build"
static_lib := "lib" + name + ".a"
shared_lib := "lib" + name + ".so"

# Default values for variables
tests := "false"
examples := "true"
build_type := "debugoptimized"

# Set default recipe to run when just is called without arguments
default: build compile

# Main tasks
build: configure compile

configure:
    @meson setup -Dtests={{tests}} -Dexamples={{examples}} -Dbuildtype={{build_type}} {{build_dir}}
    @ln -sf {{build_dir}}/{{static_lib}} {{static_lib}} 2>/dev/null || true
    @ln -sf {{build_dir}}/{{shared_lib}} {{shared_lib}} 2>/dev/null || true

reconfigure:
    @meson setup --reconfigure -Dtests={{tests}} -Dexamples={{examples}} -Dbuildtype={{build_type}} {{build_dir}}

compile:
    @meson compile -C {{build_dir}}

# Clean tasks
clean:
    @rm -f {{static_lib}} {{shared_lib}}
    @echo "{{static_lib}} and {{shared_lib}} removed ✅"

fclean: clean
    @rm -rf {{build_dir}}
    @echo "Build directory cleaned ✅"

re: reconfigure compile

# Build variants
debug:
    @just build_type="debug" reconfigure compile

release:
    @just build_type="release" reconfigure compile

# Test targets
test:
    @just tests="true" reconfigure
    @meson test -C {{build_dir}} -v
    @echo "Tests completed ✅"

# Build and list examples
examples:
    @just examples="true" reconfigure compile
    @echo ""
    @echo "\033[1;32mExamples built successfully ✅\033[0m"
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"
    @echo "\033[1;33mAvailable Examples:\033[0m"
    @echo ""
    @bash -c 'find {{build_dir}}/examples -type f -executable -not -name "*.p" | sort | while read example; do \
        name=$(basename $example); \
        case $name in \
            basic_usage)          desc="Basic flags, string, and integer options" ;; \
            subcommands)          desc="Git-like subcommands implementation" ;; \
            positional_args)      desc="Handling positional arguments" ;; \
            custom_handlers)      desc="Creating custom option handlers" ;; \
            advanced_options)     desc="Dependencies, conflicts, and exclusive groups" ;; \
            nested_commands)      desc="Complex nested subcommands (like git or docker)" ;; \
            validators)           desc="Built-in and custom validators for options" ;; \
            multi_values)         desc="Handling multiple values for an option" ;; \
            env_vars)             desc="Reading options from environment variables" ;; \
            *)                    desc="Example program" ;; \
        esac; \
        printf "  \033[1;36m%-20s\033[0m %s\n" "$name" "$desc"; \
    done'
    @echo ""
    @echo "\033[1;33mHow to run:\033[0m"
    @echo "  \033[1;37m{{build_dir}}/examples/basic_usage --help\033[0m"
    @echo "  \033[1;37m{{build_dir}}/examples/nested_commands service create --help\033[0m"
    @echo "\033[1;34m═════════════════════════════════════════════════\033[0m"

# Code quality
format:
    @echo "Formatting code with clang-format..."
    @find source includes -name "*.c" -o -name "*.h" | xargs clang-format -i
    @echo "Code formatting complete ✅"

lint:
    @echo "Running static analysis with clang-tidy..."
    @find source -name "*.c" | xargs clang-tidy -p {{build_dir}} --quiet
    @echo "Static analysis complete ✅"

# Installation
install:
    @meson install -C {{build_dir}}
    @echo "Installation complete ✅"

uninstall:
    @meson --internal uninstall -C {{build_dir}}
    @echo "Uninstallation complete ✅"

# Simple test script
quick-test: compile
    @echo "Running basic tests..."
    @bash -c '{{build_dir}}/examples/basic_usage --help >/dev/null && echo "✅ basic_usage --help works"'
    @bash -c '{{build_dir}}/examples/subcommands --help >/dev/null && echo "✅ subcommands --help works"'
    @echo "Quick tests passed ✅"

# Help function that lists available recipes
help:
    @just --list
