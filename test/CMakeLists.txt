project(c-maven-utils-tests CXX)

# Set includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test
    ${gtest_INCLUDE_DIRS}
)

add_executable(test-driver
    driver.cc
    version-test.cc
)

target_link_libraries(test-driver
    gtest
    maven_utils
    pthread
)
