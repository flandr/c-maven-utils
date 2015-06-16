project(c-maven-utils C)

# Define the public include directories
set(c-maven-utils_PUBLIC_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/c-maven-utils
    CACHE INTERNAL "c-maven-utils public include directories"
)

# The shared library target
set(c-maven-utils_SHARED_LIBRARY
    maven_utils
    CACHE INTERNAL "c-maven-utils shared library"
)

# Static has a different name, for Windows
set(c-maven-utils_STATIC_LIBRARY
    maven_utils_s
    CACHE INTERNAL "c-maven-utils static library"
)

# Source translation units
set(libmaven_utils_SRCS
    comparable-version.c
    maven-version.c
)

# Main library target
add_library(${c-maven-utils_SHARED_LIBRARY} SHARED ${libmaven_utils_SRCS})
add_library(${c-maven-utils_STATIC_LIBRARY} STATIC ${libmaven_utils_SRCS})

# Installation
install(TARGETS
    ${c-maven-utils_SHARED_LIBRARY}
    DESTINATION lib
    EXPORT c-maven-utils-targets)
install(EXPORT c-maven-utils-targets DESTINATION lib)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/c-maven-utils
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
