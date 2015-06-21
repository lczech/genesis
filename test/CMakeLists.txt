# ------------------------------------------------------------------------------
#   Setup
# ------------------------------------------------------------------------------

find_package (GTest)

if(NOT GTEST_FOUND)
    message (STATUS "Gtest not found")
    message (WARNING "${ColorRed}Cannot build tests.${ColorEnd}")
    return()
endif()

message (STATUS "Found Gtest")
message (STATUS "${ColorBlue}Building tests${ColorEnd}")

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/bin)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

file (GLOB genesis_test_sources
    ${PROJECT_SOURCE_DIR}/test/src/*.cpp
    ${PROJECT_SOURCE_DIR}/test/src/*/*.cpp
)

include_directories (${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/test/src)

# ------------------------------------------------------------------------------
#   Build Tests
# ------------------------------------------------------------------------------

include_directories   (${GTEST_INCLUDE_DIRS})

add_executable        (genesis_tests ${genesis_test_sources})
set_target_properties (genesis_tests PROPERTIES OUTPUT_NAME test_suite)

target_link_libraries (genesis_tests genesis_lib_shared)
target_link_libraries (genesis_tests ${GTEST_BOTH_LIBRARIES} -pthread)

# Need to specify the whole path here. For some reason, cmake does not
# extract it from the executable name here...
add_test (test_suite ${PROJECT_SOURCE_DIR}/test/bin/test_suite)
