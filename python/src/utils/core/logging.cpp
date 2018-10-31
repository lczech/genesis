/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::LoggingScopeLevel, scope )
{

    // -------------------------------------------------------------------
    //     Class LoggingScopeLevel
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::LoggingScopeLevel, std::shared_ptr<::genesis::utils::LoggingScopeLevel> > ( scope, "LoggingScopeLevel" )
        .def(
            pybind11::init< Logging::LoggingLevel >(),
            pybind11::arg("scope_level")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::Logging, scope )
{

    // -------------------------------------------------------------------
    //     Class Logging
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Logging, std::shared_ptr<::genesis::utils::Logging> > ( scope, "Logging" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< const Logging & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "get",
            ( std::ostringstream & ( ::genesis::utils::Logging::* )( const std::string &, const int, const std::string &, const ::genesis::utils::Logging::LoggingLevel ))( &::genesis::utils::Logging::get ),
            pybind11::arg("file"),
            pybind11::arg("line"),
            pybind11::arg("function"),
            pybind11::arg("level")
        )
        .def(
            "get",
            ( std::ostringstream & ( ::genesis::utils::Logging::* )( const std::string &, const int, const std::string &, const ::genesis::utils::Logging::LoggingLevel, const LoggingDetails ))( &::genesis::utils::Logging::get ),
            pybind11::arg("file"),
            pybind11::arg("line"),
            pybind11::arg("function"),
            pybind11::arg("level"),
            pybind11::arg("dets")
        )
        .def_static(
            "level_to_string",
            ( std::string ( * )( const ::genesis::utils::Logging::LoggingLevel ))( &::genesis::utils::Logging::level_to_string ),
            pybind11::arg("level")
        )
        .def_static(
            "log_debug",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_debug_1",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_1 ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_debug_2",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_2 ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_debug_3",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_3 ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_debug_4",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_4 ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_error",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_error ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_info",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_info ),
            pybind11::arg("msg")
        )
        .def_static(
            "log_to_file",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_to_file ),
            pybind11::arg("fn")
        )
        .def_static(
            "log_to_stdout",
            ( void ( * )(  ))( &::genesis::utils::Logging::log_to_stdout )
        )
        .def_static(
            "log_to_stream",
            ( void ( * )( std::ostream & ))( &::genesis::utils::Logging::log_to_stream ),
            pybind11::arg("os")
        )
        .def_static(
            "log_warning",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_warning ),
            pybind11::arg("msg")
        )
        .def_static(
            "max_level",
            ( ::genesis::utils::Logging::LoggingLevel ( * )(  ))( &::genesis::utils::Logging::max_level ),
            get_docstring("static LoggingLevel ::genesis::utils::Logging::max_level ()")
        )
        .def_static(
            "max_level",
            ( void ( * )( const ::genesis::utils::Logging::LoggingLevel ))( &::genesis::utils::Logging::max_level ),
            pybind11::arg("level")
        )
        .def_static(
            "report_percentage",
            ( int ( * )(  ))( &::genesis::utils::Logging::report_percentage ),
            get_docstring("static int ::genesis::utils::Logging::report_percentage ()")
        )
        .def_static(
            "report_percentage",
            ( void ( * )( const int ))( &::genesis::utils::Logging::report_percentage ),
            pybind11::arg("percentage")
        )
    ;
}

PYTHON_EXPORT_CLASS( ::genesis::utils::LoggingDetails, scope )
{

    // -------------------------------------------------------------------
    //     Class LoggingDetails
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::LoggingDetails, std::shared_ptr<::genesis::utils::LoggingDetails> > ( scope, "LoggingDetails" )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_core_logging_export, ::genesis::utils, scope )
{

    scope.def(
        "logging_progress_value",
        ( long ( * )( long ))( &::genesis::utils::logging_progress_value ),
            pybind11::arg("value")=(long)(-1),
        get_docstring("long ::genesis::utils::logging_progress_value (long value=-1)")
    );
}
