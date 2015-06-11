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

# Source translation units
set(libmaven_utils_SRCS
    maven-version.c
)

# Main library targets
add_library(${c-maven-utils_SHARED_LIBRARY} SHARED ${libmaven_utils_SRCS})

# Link dependencies
#target_link_libraries(${c-maven-utils_SHARED_LIBRARY}
#${EXTRA_LIBS}
#)

# Installation
install(TARGETS
    ${c-maven-utils_SHARED_LIBRARY}
    DESTINATION lib
    EXPORT c-maven-utils-targets)
install(EXPORT c-maven-utils-targets DESTINATION lib)

install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/c-maven-utils
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
