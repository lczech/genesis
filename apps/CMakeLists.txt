# ------------------------------------------------------------------------------
#   Build Options
# ------------------------------------------------------------------------------

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )

# ------------------------------------------------------------------------------
#   Sources
# ------------------------------------------------------------------------------

# Find all source files in this directory and compile them into individual binaries, each
# linked to the shared lib. All files should thus contain their own main function.
file( GLOB genesis_apps_sources ${PROJECT_SOURCE_DIR}/apps/*.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/lib )

# ------------------------------------------------------------------------------
#   Build Applications
# ------------------------------------------------------------------------------

if( genesis_apps_sources )

    message( STATUS "${ColorBlue}Building applications:${ColorEnd}" )
    foreach( app_src ${genesis_apps_sources} )
        get_filename_component( app_name ${app_src} NAME_WE )
        message( STATUS "  ${ColorBlue}${app_name}${ColorEnd}" )

        add_executable( ${app_name} ${app_src} )

        target_link_libraries( ${app_name} genesis_lib_shared )
        set_target_properties ( ${app_name} PROPERTIES INSTALL_RPATH "$$ORIGIN" )
    endforeach()

else()

    message( STATUS "No genesis applications found" )

endif()
