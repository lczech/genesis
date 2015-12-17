# ------------------------------------------------------------------------------
#   Setup
# ------------------------------------------------------------------------------

IF(USE_THREADS)
    set (CMAKE_THREAD_PREFER_PTHREAD ON)
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads)
ENDIF()

if(THREADS_FOUND)
    message (STATUS "Using threads")
    add_definitions( "-DPTHREADS" )
endif()

# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/bin)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files. The CMake documentation does not recommend this,
# but for our purposes, this is good enough for now.
file (GLOB_RECURSE genesis_lib_sources   ${PROJECT_SOURCE_DIR}/lib/*.cpp)
# file (GLOB_RECURSE genesis_lib_headers   ${PROJECT_SOURCE_DIR}/lib/*.hpp)
# file (GLOB_RECURSE genesis_lib_templates ${PROJECT_SOURCE_DIR}/lib/*.tpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
#   Build Libraries
# ------------------------------------------------------------------------------

if (BUILD_SHARED_LIB)
    message (STATUS "${ColorBlue}Building shared lib${ColorEnd}")
    add_library           (genesis_lib_shared SHARED ${genesis_lib_sources})
    set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)

    if(THREADS_FOUND)
        target_link_libraries (genesis_lib_shared ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()

if (BUILD_STATIC_LIB)
    message (STATUS "${ColorBlue}Building static lib${ColorEnd}")
    add_library           (genesis_lib_static STATIC ${genesis_lib_sources})
    set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)

    if(THREADS_FOUND)
        target_link_libraries (genesis_lib_static ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()
