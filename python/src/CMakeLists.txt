# ------------------------------------------------------------------------------
#   Setup
# ------------------------------------------------------------------------------

if (USE_SHARED_BOOST)
    set (Boost_USE_STATIC_LIBS OFF)
else()
    set (Boost_USE_STATIC_LIBS ON)
endif()

find_package (PythonLibs 2.7)
find_package (Boost      1.55.0 COMPONENTS python)

if (NOT PYTHONLIBS_FOUND OR NOT Boost_PYTHON_FOUND)
    message (STATUS "Python Lib or Boost Python not found")
    message (WARNING "${ColorRed}Cannot build Python module.${ColorEnd}")
    return()
endif()

message (STATUS "Found Python Lib and Boost Python")
message (STATUS "${ColorBlue}Building Python module${ColorEnd}")
message (STATUS "Using boost shared libs: " ${USE_SHARED_BOOST} )

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/python/module)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files. The CMake documentation does not recommend this,
# but for our purposes, this is good enough for now.
file (GLOB_RECURSE genesis_python_sources ${PROJECT_SOURCE_DIR}/python/src/*.cpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)
include_directories (${PROJECT_SOURCE_DIR})

# ------------------------------------------------------------------------------
#   Build Python Module
# ------------------------------------------------------------------------------

# The boost headers contain some warnings about unused variables, which does not
# look nice in the build process. For some reason, everything works find without
# this line, so we just leave it out - for now at least.
# include_directories   (${Boost_INCLUDE_DIRS})
link_directories      (${Boost_LIBRARY_DIRS})

include_directories   (${PYTHON_INCLUDE_DIRS})
link_directories      (${PYTHON_LIBRARIES})

add_library           (genesis_python_module MODULE ${genesis_python_sources})

target_link_libraries (genesis_python_module genesis_lib_shared)
target_link_libraries (genesis_python_module ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

set_target_properties (genesis_python_module PROPERTIES OUTPUT_NAME genesis)
set_target_properties (genesis_python_module PROPERTIES PREFIX "")
# set_target_properties (genesis_python_module PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/module
# )

# add_custom_command (OUTPUT python_init COMMAND
#     touch ${PROJECT_SOURCE_DIR}/module/__init__.py
# )

# If the user specified to use a shared object for boost instead of statically
# linking against it, we want to add a proper rpath to the library, so that the
# boost lib will be found if put in the same directory as the genesis lib.
if (USE_SHARED_BOOST)
    set_target_properties (genesis_python_module PROPERTIES INSTALL_RPATH "$$ORIGIN")
endif()
