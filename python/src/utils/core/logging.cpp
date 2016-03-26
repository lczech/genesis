/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (LoggingScopeLevel, "utils")
{

    // -------------------------------------------------------------------
    //     Class LoggingScopeLevel
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::LoggingScopeLevel > ( "LoggingScopeLevel", boost::python::init< Logging::LoggingLevel >(( boost::python::arg("scope_level") )) )
    ;
}

PYTHON_EXPORT_CLASS (Logging, "utils")
{

    // -------------------------------------------------------------------
    //     Class Logging
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Logging > ( "Logging", boost::python::init<  >(  ) )
        .def( boost::python::init< const Logging & >(( boost::python::arg("") )) )

        // Public Member Functions

        // .def(
        //     "get",
        //     ( std::ostringstream & ( ::genesis::utils::Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel ))( &::genesis::utils::Logging::get ),
        //     ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("std::ostringstream & ::genesis::utils::Logging::get (const std::string & file, const int line, const std::string & function, const LoggingLevel level)")
        // )
        // .def(
        //     "get",
        //     ( std::ostringstream & ( ::genesis::utils::Logging::* )( const std::string &, const int, const std::string &, const LoggingLevel, const LoggingDetails ))( &::genesis::utils::Logging::get ),
        //     ( boost::python::arg("file"), boost::python::arg("line"), boost::python::arg("function"), boost::python::arg("level"), boost::python::arg("dets") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     get_docstring("std::ostringstream & ::genesis::utils::Logging::get (const std::string & file, const int line, const std::string & function, const LoggingLevel level, const LoggingDetails dets)")
        // )
        // .def(
        //     "level_to_string",
        //     ( std::string ( * )( const LoggingLevel ))( &::genesis::utils::Logging::level_to_string ),
        //     ( boost::python::arg("level") ),
        //     get_docstring("static std::string ::genesis::utils::Logging::level_to_string (const LoggingLevel level)")
        // )
        // .staticmethod("level_to_string")
        .def(
            "log_debug",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug")
        .def(
            "log_debug_1",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_1 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_1")
        .def(
            "log_debug_2",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_2 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_2")
        .def(
            "log_debug_3",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_3 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_3")
        .def(
            "log_debug_4",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_debug_4 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_4")
        .def(
            "log_error",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_error ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_error")
        .def(
            "log_info",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_info ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_info")
        .def(
            "log_to_file",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_to_file ),
            ( boost::python::arg("fn") ),
            get_docstring("static void ::genesis::utils::Logging::log_to_file (const std::string & fn)")
        )
        .staticmethod("log_to_file")
        .def(
            "log_to_stdout",
            ( void ( * )(  ))( &::genesis::utils::Logging::log_to_stdout ),
            get_docstring("static void ::genesis::utils::Logging::log_to_stdout ()")
        )
        .staticmethod("log_to_stdout")
        .def(
            "log_to_stream",
            ( void ( * )( std::ostream & ))( &::genesis::utils::Logging::log_to_stream ),
            ( boost::python::arg("os") ),
            get_docstring("static void ::genesis::utils::Logging::log_to_stream (std::ostream & os)")
        )
        .staticmethod("log_to_stream")
        .def(
            "log_warning",
            ( void ( * )( const std::string & ))( &::genesis::utils::Logging::log_warning ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_warning")
        // .def(
        //     "max_level",
        //     ( LoggingLevel ( * )(  ))( &::genesis::utils::Logging::max_level ),
        //     get_docstring("static LoggingLevel ::genesis::utils::Logging::max_level ()")
        // )
        // .staticmethod("max_level")
        // .def(
        //     "max_level",
        //     ( void ( * )( const LoggingLevel ))( &::genesis::utils::Logging::max_level ),
        //     ( boost::python::arg("level") ),
        //     get_docstring("static void ::genesis::utils::Logging::max_level (const LoggingLevel level)")
        // )
        // .staticmethod("max_level")
        .def(
            "report_percentage",
            ( int ( * )(  ))( &::genesis::utils::Logging::report_percentage ),
            get_docstring("static int ::genesis::utils::Logging::report_percentage ()")
        )
        // .staticmethod("report_percentage")
        .def(
            "report_percentage",
            ( void ( * )( const int ))( &::genesis::utils::Logging::report_percentage ),
            ( boost::python::arg("percentage") ),
            get_docstring("static void ::genesis::utils::Logging::report_percentage (const int percentage)")
        )
        .staticmethod("report_percentage")
    ;
}

PYTHON_EXPORT_CLASS (LoggingDetails, "utils")
{

    // -------------------------------------------------------------------
    //     Class LoggingDetails
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::LoggingDetails > ( "LoggingDetails" )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_core_logging_export, "utils")
{

    boost::python::def(
        "LoggingProgressValue",
        ( long ( * )( long ))( &::genesis::utils::LoggingProgressValue ),
        ( boost::python::arg("value")=(long)(-1) ),
        get_docstring("long ::genesis::utils::LoggingProgressValue (long value=-1)")
    );
}
