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
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogError ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogError")
        .def(
            "LogWarning",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogWarning ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogWarning")
        .def(
            "LogInfo",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogInfo ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogInfo")
        .def(
            "LogDebug",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogDebug ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug")
        .def(
            "LogDebug1",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogDebug1 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug1")
        .def(
            "LogDebug2",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogDebug2 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug2")
        .def(
            "LogDebug3",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogDebug3 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug3")
        .def(
            "LogDebug4",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogDebug4 ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogDebug4")

        .def(
            "LogToStdout",
            (void ( * )(  ))( &::genesis::Logging::LogToStdout )
        )
        .staticmethod("LogToStdout")
        .def(
            "LogToFile",
            (void ( * )( ::std::string const & ))( &::genesis::Logging::LogToFile ),
            ( boost::python::arg("msg") )
        )
        .staticmethod("LogToFile")
    ;

}
