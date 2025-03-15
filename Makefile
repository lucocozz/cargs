NAME = cargs
BUILD_DIR = .build
STATIC_LIB = lib$(NAME).a
SHARED_LIB = lib$(NAME).so

# Build options
TESTS ?= false
EXAMPLES ?= true
BUILD_TYPE ?= debugoptimized

# Basic Meson options
MESON_OPTIONS = -Dtests=$(TESTS) -Dexamples=$(EXAMPLES) -Dbuildtype=$(BUILD_TYPE)

# Commands (requires Meson ≥ 1.0.0)
MESON = meson
NINJA = ninja

# Main targets
.PHONY: all build configure compile clean fclean re test examples help debug release format lint uninstall

all: build compile

# Build targets
build: configure compile

configure:
	$(MESON) setup $(MESON_OPTIONS) $(BUILD_DIR)
	@ln -sf $(BUILD_DIR)/$(STATIC_LIB) $(STATIC_LIB) 2>/dev/null || true
	@ln -sf $(BUILD_DIR)/$(SHARED_LIB) $(SHARED_LIB) 2>/dev/null || true

reconfigure:
	$(MESON) setup --reconfigure $(MESON_OPTIONS) $(BUILD_DIR)

compile:
	$(MESON) compile -C $(BUILD_DIR)

# Clean targets
clean:
	@rm -f $(STATIC_LIB) $(SHARED_LIB)
	@echo "$(STATIC_LIB) and $(SHARED_LIB) removed ✅"

fclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned ✅"

re: reconfigure compile

# Build variants
debug: BUILD_TYPE=debug
debug: reconfigure compile

release: BUILD_TYPE=release
release: reconfigure compile

# Test targets
test: TESTS=true
test: reconfigure
	$(MESON) test -C $(BUILD_DIR) -v
	@echo "Tests completed ✅"

# Examples
examples: EXAMPLES=true
examples: reconfigure
	$(MESON) compile -C $(BUILD_DIR)
	@echo "\n\033[1;32mExamples built successfully ✅\033[0m"
	@echo "\033[1;34m═════════════════════════════════════════════════\033[0m"
	@echo "\033[1;33mAvailable Examples:\033[0m"
	@echo ""
	@# Filter only executable files, not .p files
	@find $(BUILD_DIR)/examples -type f -executable -not -name "*.p" | sort | while read example; do \
		name=$$(basename $$example); \
		case $$name in \
			basic_usage)          desc="Basic flags, string, and integer options" ;; \
			subcommands)          desc="Git-like subcommands implementation" ;; \
			positional_args)      desc="Handling positional arguments" ;; \
			custom_handlers)      desc="Creating custom option handlers" ;; \
			advanced_options)     desc="Dependencies, conflicts, and exclusive groups" ;; \
			nested_commands)      desc="Complex nested subcommands (like git or docker)" ;; \
			validators)           desc="Built-in and custom validators for options" ;; \
			*)                    desc="Example program" ;; \
		esac; \
		printf "  \033[1;36m%-20s\033[0m %s\n" $$name "$$desc"; \
	done
	@echo ""
	@echo "\033[1;33mHow to run:\033[0m"
	@echo "  \033[1;37m$(BUILD_DIR)/examples/basic_usage --help\033[0m"
	@echo "  \033[1;37m$(BUILD_DIR)/examples/nested_commands service create --help\033[0m"
	@echo "\033[1;34m═════════════════════════════════════════════════\033[0m"

# Code quality
format:
	@echo "Formatting code with clang-format..."
	@find source includes -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "Code formatting complete ✅"

lint:
	@echo "Running static analysis with clang-tidy..."
	@find source -name "*.c" | xargs clang-tidy -p $(BUILD_DIR) --quiet
	@echo "Static analysis complete ✅".

# Installation
install:
	$(MESON) install -C $(BUILD_DIR)
	@echo "Installation complete ✅"

uninstall:
	$(MESON) --internal uninstall -C $(BUILD_DIR)
	@echo "Uninstallation complete ✅"

# Help
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all          - build and compile"
	@echo "  build        - build project"
	@echo "  reconfigure  - reconfigure project"
	@echo "  compile      - compile project"
	@echo "  debug        - build with debug symbols"
	@echo "  release      - build optimized release version"
	@echo "  clean        - remove library"
	@echo "  fclean       - clean build directory and remove library"
	@echo "  re           - reconfigure and compile"
	@echo "  test         - run tests with verbose output"
	@echo "  examples     - build example programs"
	@echo "  format       - format code with clang-format"
	@echo "  lint         - run static analysis with clang-tidy"
	@echo "  install      - install library and headers"
	@echo "  uninstall    - uninstall library and headers"
	@echo "  help         - print this help message"
	@echo ""
	@echo "Options:"
	@echo "  TESTS=true|false       - Enable or disable tests (default: $(TESTS))"
	@echo "  EXAMPLES=true|false    - Enable or disable examples (default: $(EXAMPLES))"
	@echo "  BUILD_TYPE=<type>      - Set build type: debug, release, debugoptimized (default: $(BUILD_TYPE))"
