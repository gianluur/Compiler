cmake_minimum_required(VERSION 3.15)
project(Compiler LANGUAGES C CXX)

# Set clang as the compiler on all platforms.
if (WIN32)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
else()
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# Specify the C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the LLVM package.
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include directory: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM libraries: ${LLVM_LIBRARIES}")

# Apply LLVM definitions.
add_definitions(${LLVM_DEFINITIONS})

# Collect your source files.
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/frontend/*.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/*.cpp
    ${CMAKE_SOURCE_DIR}/src/includes/nodes/*.cpp
)

# Create the executable target.
add_executable(${PROJECT_NAME} ${SOURCES})

# Map the required LLVM components to library names.
llvm_map_components_to_libnames(LLVM_LIBS
    core
    irreader
    support
    analysis
    passes
)

# Link LLVM libraries to your target.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LLVM_LIBS})

# Set include directories.
# Mark LLVM include directories as SYSTEM to suppress warnings from LLVM headers.
target_include_directories(${PROJECT_NAME} 
    SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS}
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/includes
)

# Set compile options only for your target.
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Find and link threading libraries.
find_package(Threads REQUIRED)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE shell32 ${CMAKE_THREAD_LIBS_INIT})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
endif()

# Install the executable.
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
