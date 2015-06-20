# ------------------------------------------------------------------------------
#   Cmake Init
# ------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

# ------------------------------------------------------------------------------
#   Project Definition
# ------------------------------------------------------------------------------

project (genesis CXX)

# set (genesis_VERSION_MAJOR 0)
# set (genesis_VERSION_MINOR 1)

set (CMAKE_BUILD_TYPE DEBUG)
# set (CMAKE_BUILD_TYPE RELEASE)
# set (CMAKE_VERBOSE_MAKEFILE ON)

# ------------------------------------------------------------------------------
#   Compiler and Linker Options
# ------------------------------------------------------------------------------

set (WARN_FLAGS              "-Wall -Wextra -pedantic -pedantic-errors")
set (CMAKE_CXX_FLAGS         "-std=c++11 ${WARN_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG   "-g -ggdb3 -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O3       -DNDEBUG")

# Misc options
# THREADS = -pthread -DPTHREADS
# STDCC   = clang++
# MPICC   = mpic++
# mpi: CCFLAGS += -DUSE_MPI
# LDFLAGS = -lm $(THREADS)

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

option (BUILD_SHARED_LIB    "Build shared library" ON)
option (BUILD_STATIC_LIB    "Build static library" OFF)
option (BUILD_EXECUTABLE    "Build executable"     OFF)

option (BUILD_PYTHON_MODULE "Build Python module"  ON)
option (BUILD_TESTS         "Build test suites"    ON)

option (USE_SHARED_BOOST    "Link against shared boost lib instead of static" ON)

if (BUILD_PYTHON_MODULE OR BUILD_TESTS)
    # Both the Python module and the tests use the shared lib,
    # so we need to build it if one of those is activated.
    set (BUILD_SHARED_LIB ON)
endif()

if (NOT WIN32)
    # Fun with colors!
    string(ASCII 27 Esc)
    set(ColorRed  "${Esc}[31m")
    set(ColorBlue "${Esc}[34m")
    set(ColorEnd  "${Esc}[0m")
endif()

# ------------------------------------------------------------------------------
#   Build Libraries
# ------------------------------------------------------------------------------

if (BUILD_SHARED_LIB OR BUILD_STATIC_LIB OR BUILD_EXECUTABLE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
endif()

# ------------------------------------------------------------------------------
#   Build Python Module
# ------------------------------------------------------------------------------

if (BUILD_PYTHON_MODULE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/python/src)
endif()

# ------------------------------------------------------------------------------
#   Build Tests
# ------------------------------------------------------------------------------

if (BUILD_TESTS)
    # Need to enable testing here, otherwise ./build/make test will not work.
    enable_testing()

    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
