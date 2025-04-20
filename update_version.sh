#!/bin/bash
set -e

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Default paths (can be overridden by environment variables)
CONAN_INDEX_PATH="${CONAN_INDEX_PATH:-$HOME/conan-center-index}"
VCPKG_PATH="${VCPKG_PATH:-$HOME/vcpkg}"
REPO_PATH="$(pwd)"

# Print usage information
show_help() {
    echo -e "${BLUE}${BOLD}cargs Version Updater${NC}"
    echo "This script automates the version update process for cargs."
    echo ""
    echo "Usage: $0 NEW_VERSION [options]"
    echo ""
    echo "Arguments:"
    echo "  NEW_VERSION        New version number (e.g., 1.1.0)"
    echo ""
    echo "Options:"
    echo "  --current=VERSION  Current version number (default: auto-detect)"
    echo "  --conan-path=PATH  Path to conan-center-index repo (default: $CONAN_INDEX_PATH)"
    echo "  --vcpkg-path=PATH  Path to vcpkg repo (default: $VCPKG_PATH)"
    echo "  --no-conan         Skip Conan package update"
    echo "  --no-vcpkg         Skip vcpkg package update" 
    echo "  --no-tag           Don't create Git tag"
    echo "  --dry-run          Show what would be done without making changes"
    echo "  --help             Show this help message"
    echo ""
    echo "Example: $0 1.1.0"
}

# Parse command-line arguments
NEW_VERSION=""
CURRENT_VERSION=""
SKIP_CONAN=false
SKIP_VCPKG=false
SKIP_TAG=false
DRY_RUN=false

while [ "$#" -gt 0 ]; do
    case "$1" in
        --help)
            show_help
            exit 0
            ;;
        --current=*)
            CURRENT_VERSION="${1#*=}"
            shift
            ;;
        --conan-path=*)
            CONAN_INDEX_PATH="${1#*=}"
            shift
            ;;
        --vcpkg-path=*)
            VCPKG_PATH="${1#*=}"
            shift
            ;;
        --no-conan)
            SKIP_CONAN=true
            shift
            ;;
        --no-vcpkg)
            SKIP_VCPKG=true
            shift
            ;;
        --no-tag)
            SKIP_TAG=true
            shift
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        -*)
            echo -e "${RED}Error: Unknown option $1${NC}"
            show_help
            exit 1
            ;;
        *)
            if [ -z "$NEW_VERSION" ]; then
                NEW_VERSION="$1"
            else
                echo -e "${RED}Error: Unexpected argument $1${NC}"
                show_help
                exit 1
            fi
            shift
            ;;
    esac
done

# Validate required arguments
if [ -z "$NEW_VERSION" ]; then
    echo -e "${RED}Error: New version number is required${NC}"
    show_help
    exit 1
fi

# Validate version format
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Version number must be in format X.Y.Z${NC}"
    exit 1
fi

# Auto-detect current version if not specified
if [ -z "$CURRENT_VERSION" ]; then
    # Try to detect from meson.build
    CURRENT_VERSION=$(grep "version: '[0-9.]*'," "meson.build" | grep -Eo "[0-9]+\.[0-9]+\.[0-9]+" | head -1)
    
    if [ -z "$CURRENT_VERSION" ]; then
        echo -e "${RED}Error: Could not auto-detect current version. Please specify with --current=VERSION${NC}"
        exit 1
    fi
fi

echo -e "${BLUE}${BOLD}cargs Version Update${NC}"
echo -e "Current version: ${YELLOW}$CURRENT_VERSION${NC}"
echo -e "New version:     ${GREEN}$NEW_VERSION${NC}"
echo ""

# Ensure repository is clean
if [ "$DRY_RUN" = false ]; then
    if [ -n "$(git status --porcelain)" ]; then
        echo -e "${RED}Error: Working directory is not clean. Please commit or stash changes.${NC}"
        exit 1
    fi
fi

# Function to execute or print command in dry run mode
exec_cmd() {
    if [ "$DRY_RUN" = true ]; then
        echo -e "${YELLOW}Would run: $*${NC}"
    else
        echo -e "${BLUE}Running: ${BOLD}$*${NC}"
        "$@"
    fi
}

# 1. Update version strings in repository files
echo -e "${BLUE}${BOLD}Updating version in repository files...${NC}"

# Update meson.build
exec_cmd sed -i "s/version: '$CURRENT_VERSION'/version: '$NEW_VERSION'/g" "meson.build"

# Update installers
exec_cmd sed -i "s/VERSION=\"$CURRENT_VERSION\"/VERSION=\"$NEW_VERSION\"/g" "packaging/installers/install.sh"

# Update packaging files
# Conan
exec_cmd sed -i "s/version = \"$CURRENT_VERSION\"/version = \"$NEW_VERSION\"/g" "packaging/conan/conanfile.py"

# vcpkg
exec_cmd sed -i "s/\"version\": \"$CURRENT_VERSION\"/\"version\": \"$NEW_VERSION\"/g" "packaging/vcpkg/vcpkg.json"

# 2. Commit changes
if [ "$DRY_RUN" = false ]; then
    echo -e "${BLUE}${BOLD}Committing changes...${NC}"
    exec_cmd git add .
    exec_cmd git commit -m "Bump version to $NEW_VERSION"
else
    echo -e "${YELLOW}Would commit changes with message: Bump version to $NEW_VERSION${NC}"
fi

# 3. Create and push tag if requested
if [ "$SKIP_TAG" = false ]; then
    echo -e "${BLUE}${BOLD}Creating version tag...${NC}"
    if [ "$DRY_RUN" = false ]; then
        exec_cmd git tag -a "v$NEW_VERSION" -m "Version $NEW_VERSION"
        exec_cmd git push origin "v$NEW_VERSION"
    else
        echo -e "${YELLOW}Would create tag: v$NEW_VERSION${NC}"
        echo -e "${YELLOW}Would push tag to origin${NC}"
    fi
fi

# 4. Download and get hash of the new tag archive
echo -e "${BLUE}${BOLD}Downloading release archive to compute hash...${NC}"
TEMP_DIR=$(mktemp -d)
ARCHIVE_URL="https://github.com/lucocozz/cargs/archive/refs/tags/v$NEW_VERSION.tar.gz"
ARCHIVE_PATH="$TEMP_DIR/v$NEW_VERSION.tar.gz"

if [ "$DRY_RUN" = false ]; then
    # Small delay to ensure GitHub has the tag available
    sleep 2
    
    # Try to download the archive
    echo "Downloading from: $ARCHIVE_URL"
    for i in {1..5}; do
        if curl -sL "$ARCHIVE_URL" -o "$ARCHIVE_PATH"; then
            if [ -f "$ARCHIVE_PATH" ] && [ -s "$ARCHIVE_PATH" ]; then
                echo "Download successful"
                break
            fi
        fi
        echo "Attempt $i failed, retrying in 3 seconds..."
        sleep 3
    done
    
    if [ ! -f "$ARCHIVE_PATH" ] || [ ! -s "$ARCHIVE_PATH" ]; then
        echo -e "${RED}Error: Failed to download archive after multiple attempts${NC}"
        echo "This might be because the tag is not yet available on GitHub."
        echo "Please run the script again later or update the SHA manually."
        exit 1
    fi
    
    # Compute hash
    SHA512=$(shasum -a 512 "$ARCHIVE_PATH" | awk '{print $1}')
    echo "SHA512: $SHA512"
else
    echo -e "${YELLOW}Would download archive from: $ARCHIVE_URL${NC}"
    echo -e "${YELLOW}Would compute SHA512 hash${NC}"
    SHA512="SAMPLE_HASH_FOR_DRY_RUN"
fi

# 5. Update package managers

# Update Conan package
if [ "$SKIP_CONAN" = false ]; then
    echo -e "${BLUE}${BOLD}Updating Conan package...${NC}"
    
    if [ ! -d "$CONAN_INDEX_PATH" ]; then
        echo -e "${RED}Error: Conan Center Index repository not found at $CONAN_INDEX_PATH${NC}"
        echo "Please clone it or specify the correct path with --conan-path"
        exit 1
    fi
    
    # Path to recipe directory
    CONAN_RECIPE_PATH="$CONAN_INDEX_PATH/recipes/libcargs"
    
    if [ ! -d "$CONAN_RECIPE_PATH" ]; then
        echo -e "${RED}Error: libcargs recipe not found in Conan Center Index${NC}"
        echo "Please ensure the recipe exists and you have the correct repository"
        exit 1
    fi
    
    # Update conandata.yml with new version and hash
    CONAN_DATA_PATH="$CONAN_RECIPE_PATH/all/conandata.yml"
    
    if [ "$DRY_RUN" = false ]; then
        # Create a backup
        cp "$CONAN_DATA_PATH" "$CONAN_DATA_PATH.bak"
        
        # Check if version already exists
        if grep -q "\"$NEW_VERSION\":" "$CONAN_DATA_PATH"; then
            # Update existing entry
            sed -i "/\"$NEW_VERSION\":/,/sha256:/ s/sha256: .*/sha256: \"$SHA512\"/" "$CONAN_DATA_PATH"
        else
            # Add new version entry at the top
            awk -v ver="$NEW_VERSION" -v hash="$SHA512" '
                /^sources:/ { 
                    print $0; 
                    print "  \"" ver "\":"; 
                    print "    url: \"https://github.com/lucocozz/cargs/archive/refs/tags/v" ver ".tar.gz\""; 
                    print "    sha256: \"" hash "\""; 
                    next; 
                }
                { print $0 }
            ' "$CONAN_DATA_PATH" > "$CONAN_DATA_PATH.new"
            
            mv "$CONAN_DATA_PATH.new" "$CONAN_DATA_PATH"
        fi
        
        # Update conanfile.py with new version if needed
        CONAN_FILE_PATH="$CONAN_RECIPE_PATH/all/conanfile.py"
        if [ -f "$CONAN_FILE_PATH" ]; then
            if grep -q "name = \"libcargs\"" "$CONAN_FILE_PATH"; then
                sed -i "s/version = \"[0-9.]*\"/version = \"$NEW_VERSION\"/" "$CONAN_FILE_PATH"
            fi
        fi
        
        echo -e "${GREEN}Conan package updated successfully${NC}"
    else
        echo -e "${YELLOW}Would update Conan recipe at: $CONAN_RECIPE_PATH${NC}"
        echo -e "${YELLOW}Would update conandata.yml with new version and hash${NC}"
        echo -e "${YELLOW}Would update conanfile.py with new version${NC}"
    fi
fi

# Update vcpkg package
if [ "$SKIP_VCPKG" = false ]; then
    echo -e "${BLUE}${BOLD}Updating vcpkg package...${NC}"
    
    if [ ! -d "$VCPKG_PATH" ]; then
        echo -e "${RED}Error: vcpkg repository not found at $VCPKG_PATH${NC}"
        echo "Please clone it or specify the correct path with --vcpkg-path"
        exit 1
    fi
    
    # Path to port directory
    VCPKG_PORT_PATH="$VCPKG_PATH/ports/libcargs"
    
    if [ ! -d "$VCPKG_PORT_PATH" ]; then
        echo -e "${RED}Error: libcargs port not found in vcpkg${NC}"
        echo "Please ensure the port exists and you have the correct repository"
        exit 1
    fi
    
    # Update portfile.cmake with new version and hash
    PORTFILE_PATH="$VCPKG_PORT_PATH/portfile.cmake"
    
    if [ "$DRY_RUN" = false ]; then
        # Create a backup
        cp "$PORTFILE_PATH" "$PORTFILE_PATH.bak"
        
        # Update SHA512 hash
        sed -i "s/SHA512 [a-fA-F0-9]*/SHA512 $SHA512/" "$PORTFILE_PATH"
        
        # Update version
        if grep -q "REF v\${VERSION}" "$PORTFILE_PATH"; then
            # Version is referenced as a variable, check vcpkg.json
            VCPKG_JSON_PATH="$VCPKG_PORT_PATH/vcpkg.json"
            if [ -f "$VCPKG_JSON_PATH" ]; then
                sed -i "s/\"version\": \"[0-9.]*\"/\"version\": \"$NEW_VERSION\"/" "$VCPKG_JSON_PATH"
            fi
        else
            # Direct version reference
            sed -i "s/REF v[0-9.]\+/REF v$NEW_VERSION/" "$PORTFILE_PATH"
        fi
        
        echo -e "${GREEN}vcpkg package updated successfully${NC}"
    else
        echo -e "${YELLOW}Would update vcpkg port at: $VCPKG_PORT_PATH${NC}"
        echo -e "${YELLOW}Would update portfile.cmake with new version and hash${NC}"
        echo -e "${YELLOW}Would update vcpkg.json if needed${NC}"
    fi
fi

# Update repository with the new hash
echo -e "${BLUE}${BOLD}Updating repository with hash...${NC}"

# Update packaging files with SHA512 hash
# Conan
exec_cmd sed -i "s/sha256: \"\"/sha256: \"$SHA512\"/" "packaging/conan/conandata.yml"

# vcpkg
exec_cmd sed -i "s/SHA512 [a-fA-F0-9]*/SHA512 $SHA512/" "packaging/vcpkg/portfile.cmake"

# Commit and push hash update
if [ "$DRY_RUN" = false ]; then
    echo -e "${BLUE}${BOLD}Committing hash updates...${NC}"
    exec_cmd git add .
    exec_cmd git commit -m "Update SHA512 hash for version $NEW_VERSION"
    exec_cmd git push
else
    echo -e "${YELLOW}Would commit hash updates with message: Update SHA512 hash for version $NEW_VERSION${NC}"
    echo -e "${YELLOW}Would push changes to origin${NC}"
fi

# Clean up
if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

if [ "$DRY_RUN" = true ]; then
    echo -e "${GREEN}${BOLD}Dry run completed successfully!${NC}"
    echo "Run without --dry-run to apply the changes."
else
    echo -e "${GREEN}${BOLD}Version update completed successfully!${NC}"
    echo -e "Updated from ${YELLOW}$CURRENT_VERSION${NC} to ${GREEN}$NEW_VERSION${NC}"
    
    # Print reminder to create pull requests for package managers
    if [ "$SKIP_CONAN" = false ]; then
        echo -e "${BLUE}Next steps for Conan:${NC}"
        echo "1. cd $CONAN_INDEX_PATH"
        echo "2. git checkout -b libcargs-$NEW_VERSION"
        echo "3. git add ."
        echo "4. git commit -m \"libcargs: update to $NEW_VERSION\""
        echo "5. git push -u origin libcargs-$NEW_VERSION"
        echo "6. Create a pull request on GitHub"
    fi
    
    if [ "$SKIP_VCPKG" = false ]; then
        echo -e "${BLUE}Next steps for vcpkg:${NC}"
        echo "1. cd $VCPKG_PATH"
        echo "2. git checkout -b libcargs-$NEW_VERSION"
        echo "3. git add ."
        echo "4. git commit -m \"[libcargs] Update to version $NEW_VERSION\""
        echo "5. git push -u origin libcargs-$NEW_VERSION"
        echo "6. Create a pull request on GitHub"
    fi
fi
