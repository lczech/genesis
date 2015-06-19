# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set (LIBRARY_OUTPUT_PATH    ${PROJECT_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

file (GLOB genesis_lib_sources ${PROJECT_SOURCE_DIR}/lib/*/*.cpp)
file (GLOB genesis_lib_headers ${PROJECT_SOURCE_DIR}/lib/*/*.hpp lib/*/*.tpp)

include_directories (${PROJECT_SOURCE_DIR}/lib)

# ------------------------------------------------------------------------------
#   Build Libraries
# ------------------------------------------------------------------------------

if (BUILD_SHARED_LIB)
    message (STATUS "${ColorBlue}Building shared lib${ColorEnd}")
    add_library           (genesis_lib_shared SHARED ${genesis_lib_sources})
    set_target_properties (genesis_lib_shared PROPERTIES OUTPUT_NAME genesis)
endif()

if (BUILD_STATIC_LIB)
    message (STATUS "${ColorBlue}Building static lib${ColorEnd}")
    add_library           (genesis_lib_static STATIC ${genesis_lib_sources})
    set_target_properties (genesis_lib_static PROPERTIES OUTPUT_NAME genesis)
endif()

if (BUILD_EXECUTABLE)
    message (WARNING
        "${ColorRed}Currently, genesis does not support a single executable. "
        "Use it as a library or via the Python module instead.${ColorEnd}"
    )

    # message (STATUS "${ColorBlue}Building executable${ColorEnd}")
    # add_executable        (
    #     genesis_bin_main ${genesis_lib_sources}
    #     ${PROJECT_SOURCE_DIR}/src/main.cpp
    # )
    # set_target_properties (genesis_bin_main PROPERTIES OUTPUT_NAME genesis)
endif()
