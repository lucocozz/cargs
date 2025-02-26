NAME = cargs
BIN = lib$(NAME).a
BUILD_DIR = .build


ifndef DEBUG
	DEBUG = false
endif

ifndef TEST
	TEST = false
endif

OPTIONS = -Ddebug=$(DEBUG) -Dtest=$(TEST)



all: build compile

build:
	meson setup $(OPTIONS) $(BUILD_DIR)
	@ln -sf $(BUILD_DIR)/$(BIN) $(BIN)

reconfigure:
	meson setup --reconfigure $(OPTIONS) $(BUILD_DIR)

wipe:
	meson setup --wipe $(BUILD_DIR)

compile:
	meson compile -C $(BUILD_DIR)

clean:
	@rm -rf $(BIN)
	@echo "$(BIN) removed ✅"

fclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned ✅"

re: reconfigure compile


test:
	meson test -C $(BUILD_DIR)


help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all          - build and compile"
	@echo "  build        - build project"
	@echo "  reconfigure  - reconfigure project"
	@echo "  wipe         - wipe build directory"
	@echo "  compile      - compile project"
	@echo "  clean        - remove executable"
	@echo "  fclean       - clean build directory and remove executable"
	@echo "  re           - reconfigure and compile"
	@echo "  test         - run test"
	@echo "  help         - print this help message"


.PHONY: all build compile clean fclean re reconfigure wipe help