#!/bin/bash

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Compilation flags
CXXFLAGS="-Wall $(llvm-config --cxxflags)"
LDFLAGS="$(llvm-config --ldflags --system-libs --libs core irreader support)"

# Directories
SRC_DIR="src"
BUILD_DIR="build"
OBJ_DIR="$BUILD_DIR/obj"
NODES_DIR="$SRC_DIR/includes/nodes"
FRONTEND_DIR="$SRC_DIR/frontend"  # Add this line for the frontend directory

# Function to show help
show_help() {
    echo -e "${BLUE}Usage:${NC}"
    echo -e "  ./build.sh [options] [program arguments]"
    echo
    echo -e "${BLUE}Options:${NC}"
    echo "  -h, --help     Show this help message"
    echo "  -b, --build    Only build, don't run"
    echo "  -c, --clean    Clean build directory before building"
    echo "  -r, --run      Only run, don't build"
    echo
    echo -e "${BLUE}Examples:${NC}"
    echo "  ./build.sh             # Build and run"
    echo "  ./build.sh -b          # Only build"
    echo "  ./build.sh -c          # Clean, build, and run"
    echo "  ./build.sh arg1 arg2   # Build and run with arguments"
}

# Function to clean build directory
clean_build() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    mkdir -p "$OBJ_DIR/includes/nodes"
}

# Function to check if source file is newer than its object file
needs_compile() {
    local src=$1
    local obj=$2
    
    # If object file doesn't exist, we need to compile
    if [ ! -f "$obj" ]; then
        return 0 # true in bash
    fi
    
    # If source is newer than object file, we need to compile
    if [ "$src" -nt "$obj" ]; then
        return 0 # true in bash
    fi
    
    # Check if any header file is newer than the object file
    for header in $(find "$SRC_DIR" -name "*.hpp"); do
        if [ "$header" -nt "$obj" ]; then
            return 0 # true in bash
        fi
    done
    
    return 1 # false in bash
}

# Function to compile a single file
compile_file() {
    local src=$1
    local obj=$2
    local name=$(basename "$src")
    
    echo -e "${BLUE}Compiling ${name}...${NC}"
    if clang++ $CXXFLAGS -c "$src" -o "$obj"; then
        echo -e "${GREEN}Successfully compiled ${name}${NC}"
        return 0
    else
        echo -e "${RED}Failed to compile ${name}${NC}"
        return 1
    fi
}

# Function to build the project
build_project() {
    # Create build directories if they don't exist
    mkdir -p "$OBJ_DIR/includes/nodes"
    mkdir -p "$OBJ_DIR/frontend"  # Add this line for frontend objects

    # Track if any files were compiled
    local compiled=0

    # Check and compile main.cpp if needed
    local main_src="$SRC_DIR/main.cpp"
    local main_obj="$OBJ_DIR/main.o"

    if needs_compile "$main_src" "$main_obj"; then
        compile_file "$main_src" "$main_obj" || return 1
        compiled=1
    else
        echo -e "${GREEN}main.cpp is up to date${NC}"
    fi

    # Check and compile scope.cpp if needed
    local scope_src="$SRC_DIR/frontend/scope.cpp"
    local scope_obj="$OBJ_DIR/frontend/scope.o"

    if needs_compile "$scope_src" "$scope_obj"; then
        compile_file "$scope_src" "$scope_obj" || return 1
        compiled=1
    else
        echo -e "${GREEN}scope.cpp is up to date${NC}"
    fi

    # Check and compile all node files if needed
    for src in "$NODES_DIR"/*.cpp; do
        # Skip if no .cpp files exist
        [ -e "$src" ] || continue
        
        # Get the object file path
        local filename=$(basename "$src")
        local obj="$OBJ_DIR/includes/nodes/${filename%.cpp}.o"
        
        if needs_compile "$src" "$obj"; then
            compile_file "$src" "$obj" || return 1
            compiled=1
        else
            echo -e "${GREEN}${filename} is up to date${NC}"
        fi
    done

    # If any file was compiled or if the executable doesn't exist, we need to link
    if [ $compiled -eq 1 ] || [ ! -f "$BUILD_DIR/main" ] || [ "$OBJ_DIR/main.o" -nt "$BUILD_DIR/main" ]; then
        echo -e "${BLUE}Linking...${NC}"
        if clang++ "$OBJ_DIR/main.o" "$OBJ_DIR/frontend/scope.o" "$OBJ_DIR"/includes/nodes/*.o $LDFLAGS -o "$BUILD_DIR/main"; then
            echo -e "${GREEN}Successfully linked executable${NC}"
        else
            echo -e "${RED}Failed to link executable${NC}"
            return 1
        fi
    else
        echo -e "${GREEN}Everything is up to date${NC}"
    fi

    return 0
}


# Function to run the program
run_program() {
    if [ ! -f "$BUILD_DIR/main" ]; then
        echo -e "${RED}Error: Executable not found. Please build the project first.${NC}"
        return 1
    fi

    echo -e "${BLUE}Running program...${NC}"
    echo -e "${YELLOW}Command: $BUILD_DIR/main $@${NC}"
    echo "----------------------------------------"
    "$BUILD_DIR/main" "$@"
    local exit_code=$?
    echo "----------------------------------------"
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}Program finished successfully${NC}"
    else
        echo -e "${RED}Program exited with code ${exit_code}${NC}"
    fi
    return $exit_code
}

# Parse command line arguments
ONLY_BUILD=0
ONLY_RUN=0
CLEAN=0
PROGRAM_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -b|--build)
            ONLY_BUILD=1
            shift
            ;;
        -r|--run)
            ONLY_RUN=1
            shift
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        *)
            PROGRAM_ARGS+=("$1")
            shift
            ;;
    esac
done

# Main execution
if [ $CLEAN -eq 1 ]; then
    clean_build
fi

if [ $ONLY_RUN -eq 0 ]; then
    build_project || exit 1
fi

if [ $ONLY_BUILD -eq 0 ]; then
    run_program "${PROGRAM_ARGS[@]}"
fi