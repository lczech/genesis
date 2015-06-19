# ------------------------------------------------------------------------------
#   Setup
# ------------------------------------------------------------------------------

enable_testing()
find_package (GTest)

if(NOT GTEST_FOUND)
    message (STATUS "Gtest not found")
    message (WARNING "${ColorRed}Cannot build tests.${ColorEnd}")
    return()
endif()

message (STATUS "Found Gtest")
message (STATUS "${ColorBlue}Building tests.${ColorEnd}")

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

# ------------------------------------------------------------------------------
#   Build Tests
# ------------------------------------------------------------------------------

include_directories (${GTEST_INCLUDE_DIRS})

add_executable (genesis_tests ${genesis_test_sources})
# set_target_properties (genesis_tests PROPERTIES OUTPUT_NAME genesis)

target_link_libraries(genesis_tests genesis_lib_shared)
target_link_libraries(genesis_tests ${GTEST_BOTH_LIBRARIES} -pthread)

# add_test(genesis_tests genesis_tests --gtest_output=xml:${CMAKE_BINARY_DIR}/Testing/genesis_tests.xml)
add_test(run_tests genesis_tests)
