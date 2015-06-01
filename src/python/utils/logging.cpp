/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "utils/logging.hpp"

void BoostPythonExport_Logging()
{

    boost::python::class_< ::genesis::Logging >( "Logging", boost::python::no_init )
        .def(
            "LogError",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_error ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogError")
        .def(
            "LogWarning",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_warning ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogWarning")
        .def(
            "LogInfo",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_info ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogInfo")
        .def(
            "LogDebug",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_debug ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug")
        .def(
            "LogDebug1",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_debug_1 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug1")
        .def(
            "LogDebug2",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_debug_2 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug2")
        .def(
            "LogDebug3",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_debug_3 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug3")
        .def(
            "LogDebug4",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_debug_4 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug4")

        .def(
            "LogToStdout",
            (void ( * )(  ))( &::genesis::Logging::log_to_stdout )
        )
        .staticmethod("LogToStdout")
        .def(
            "Logto_file",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::log_to_file ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("Logto_file")
    ;

}
