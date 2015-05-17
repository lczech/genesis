# --------------------------------------------------------------------
#   Cmake Init
# --------------------------------------------------------------------

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

# --------------------------------------------------------------------
#   Project Definition
# --------------------------------------------------------------------

project (genesis CXX)

#~ set (genesis_VERSION_MAJOR 0)
#~ set (genesis_VERSION_MINOR 1)

set (CMAKE_BUILD_TYPE DEBUG)
#~ set (CMAKE_VERBOSE_MAKEFILE ON)

# --------------------------------------------------------------------
#   Compiler and Linker Options
# --------------------------------------------------------------------

set (WARN_FLAGS              "-Wall -Wextra -pedantic -pedantic-errors")
set (CMAKE_CXX_FLAGS         "-std=c++11 ${WARN_FLAGS} -I../src")

set (CMAKE_CXX_FLAGS_DEBUG   "-g -ggdb3 -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O3       -DNDEBUG")

# Misc options
#~ THREADS = -pthread -DPTHREADS
#~ STDCC   = clang++
#~ MPICC   = mpic++
#~ mpi: CCFLAGS += -DUSE_MPI
#~ LDFLAGS = -lm $(THREADS)

# --------------------------------------------------------------------
#   Build Options
# --------------------------------------------------------------------

option (BUILD_STATIC_LIB    "Build static library" OFF)
option (BUILD_SHARED_LIB    "Build shared library" OFF)

option (USE_SHARED_BOOST    "Link against shared boost lib instead of static" OFF)
option (BUILD_PYTHON_MODULE "Build Python module"  ON)
option (BUILD_EXECUTABLE    "Build executable"     OFF)

option (BUILD_TESTS         "Build test suites"    ON)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/bin)

# --------------------------------------------------------------------
#   Source
# --------------------------------------------------------------------

file (GLOB genesis_lib_sources ${PROJECT_SOURCE_DIR}/lib/*/*.cpp)
file (GLOB genesis_lib_headers ${PROJECT_SOURCE_DIR}/lib/*/*.hpp lib/*/*.tpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)

# --------------------------------------------------------------------
#   Build Libraries
# --------------------------------------------------------------------

if (BUILD_STATIC_LIB)
    add_library           (genesis_lib_static STATIC ${genesis_lib_sources})
    set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)
endif()

if (BUILD_SHARED_LIB)
    add_library           (genesis_lib_shared SHARED ${genesis_lib_sources})
    set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)
endif()

if (BUILD_EXECUTABLE)
    add_executable        (genesis_bin_main ${genesis_lib_sources} ${PROJECT_SOURCE_DIR}/src/main/main.cpp)
    set_target_properties (genesis_bin_main PROPERTIES OUTPUT_NAME genesis)
endif()

# --------------------------------------------------------------------
#   Build Python Module
# --------------------------------------------------------------------

if (BUILD_PYTHON_MODULE OR BUILD_EXECUTABLE)
    set (Boost_USE_STATIC_LIBS (NOT ${USE_SHARED_BOOST}))

    find_package (PythonLibs 2.7)
    find_package (Boost      1.55.0 COMPONENTS python)

    if (PYTHONLIBS_FOUND AND Boost_PYTHON_FOUND)
        message (STATUS "Found Python Lib and Boost Python, building Python module")

        # the boost headers contain some warnings about unused variables, which does not look
        # nice in the build process. for some reason, everything works find without this line,
        # so we just leave it out - for now at least.
        #~ include_directories   (${Boost_INCLUDE_DIRS})
        link_directories      (${Boost_LIBRARY_DIRS})

        include_directories   (${PYTHON_INCLUDE_DIRS})
        link_directories      (${PYTHON_LIBRARIES})

        file (GLOB genesis_python_sources ${PROJECT_SOURCE_DIR}/src/python/bindings.cpp ${PROJECT_SOURCE_DIR}/src/python/*/*.cpp)

        add_library           (genesis_python_module MODULE ${genesis_python_sources} ${genesis_lib_sources} python_init)
        target_link_libraries (genesis_python_module ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

        set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME genesis)
        set_target_properties (genesis_python_module PROPERTIES PREFIX "")
        set_target_properties (genesis_python_module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/module)

        add_custom_command (OUTPUT python_init COMMAND touch ${PROJECT_SOURCE_DIR}/module/__init__.py)

        # if the user specified to use a shared object for boost instead of statically linking
        # against it, we want to add a proper rpath to the library, so that the boost lib will
        # be found if put in the same directory as the genesis lib.
        if (USE_SHARED_BOOST)
            set_target_properties (genesis_python_module PROPERTIES INSTALL_RPATH "$$ORIGIN")
        endif()

        #~ if (BUILD_EXECUTABLE)
            #~ add_executable        (genesis_bin ${genesis_lib_sources} ../src/main/main.cpp)
            #~ set_target_properties (genesis_bin PROPERTIES OUTPUT_NAME genesis)
        #~ endif()
    else()
        message (STATUS "Python Lib or Boost Python not found, cannot build Python module")
    endif()
endif()
