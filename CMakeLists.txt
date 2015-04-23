# --------------------------------------------------------------------
#   Cmake Init
# --------------------------------------------------------------------

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DISABLE_SOURCE_CHANGES  ON)

# --------------------------------------------------------------------
#   Project Definition
# --------------------------------------------------------------------

project (genesis)

#~ set (genesis_VERSION_MAJOR 0)
#~ set (genesis_VERSION_MINOR 1)

#~ set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE DEBUG)

# --------------------------------------------------------------------
#   Compiler and Linker
# --------------------------------------------------------------------


#~ set (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
#~ set (CMAKE_CXX_FLAGS                "-Wall -Wextra -pedantic -pedantic-errors -std=c++11 -O3 -I../src    -I/usr/include/python2.7 -lpython2.7 -lboost_python")
set (CMAKE_CXX_FLAGS                "-Wall -Wextra -pedantic -pedantic-errors -std=c++11 -O3 -I../lib -I/usr/include/python2.7")
set (CMAKE_CXX_FLAGS_DEBUG          "-g -ggdb3 -DDEBUG")
#~ set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#~ set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#~ set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# --------------------------------------------------------------------
#   Source
# --------------------------------------------------------------------

file (GLOB genesis_lib_sources lib/*/*.cpp)

# --------------------------------------------------------------------
#   Build Targets
# --------------------------------------------------------------------

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/bin)

#~ add_executable        (genesis_bin ${genesis_lib_sources})
#~ set_target_properties (genesis_bin PROPERTIES OUTPUT_NAME genesis)

add_library           (genesis_lib_static STATIC ${genesis_lib_sources})
set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)

add_library           (genesis_lib_shared SHARED ${genesis_lib_sources})
set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)


find_package (PythonLibs 2.7)
find_package (Boost      1.55.0 COMPONENTS python)

if (PYTHONLIBS_FOUND AND BOOST_PYTHON_FOUND)

    set (Boost_USE_STATIC_LIBS   ON)

    include_directories   (${Boost_INCLUDE_DIRS})
    link_directories      (${Boost_LIBRARY_DIRS})

    include_directories   (${PYTHON_INCLUDE_DIRS})
    link_directories      (${PYTHON_LIBRARIES})

    #~ add_library           (genesis_python_module SHARED src/python/python.cpp ${genesis_lib_sources})
    add_library           (genesis_python_module SHARED src/python/python.cpp lib/utils/bitvector.cpp)
    target_link_libraries (genesis_python_module ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
    set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME module)

endif()
