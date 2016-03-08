/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/utils/core/logging.hpp"

using namespace genesis::utils;

PYTHON_EXPORT_CLASS (Logging, "utils")
{
    boost::python::class_< ::genesis::utils::Logging >( "Logging", boost::python::no_init )
        .def(
            "log_error",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_error ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_error")
        .def(
            "log_warning",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_warning ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_warning")
        .def(
            "log_info",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_info ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_info")
        .def(
            "log_debug",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_debug ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug")
        .def(
            "log_debug_1",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_debug_1 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_1")
        .def(
            "log_debug_2",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_debug_2 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_2")
        .def(
            "log_debug_3",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_debug_3 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_3")
        .def(
            "log_debug_4",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_debug_4 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("log_debug_4")

        .def(
            "log_to_stdout",
            (void ( * )(  ))( &::genesis::utils::Logging::log_to_stdout ),
            get_docstring("static void ::genesis::utils::Logging::log_to_stdout ()")
        )
        .staticmethod("log_to_stdout")
        .def(
            "log_to_file",
            (void ( * )( ::std::string const & ))( &::genesis::utils::Logging::log_to_file ),
            ( boost::python::arg("msg") ),
            get_docstring("static void ::genesis::utils::Logging::log_to_file (const std::string & fn)")
        )
        .staticmethod("log_to_file")
        .def(
            "max_level",
            ( ::genesis::utils::Logging::LoggingLevel ( * )(  ))( &::genesis::utils::Logging::max_level ),
            get_docstring("static LoggingLevel ::genesis::utils::Logging::max_level ()")
        )
        .def(
            "max_level",
            ( void ( * )( const ::genesis::utils::Logging::LoggingLevel ))( &::genesis::utils::Logging::max_level ),
            ( boost::python::arg("level") ),
            get_docstring("static void ::genesis::utils::Logging::max_level (const LoggingLevel level)")
        )
        .staticmethod("max_level")
    ;
}
