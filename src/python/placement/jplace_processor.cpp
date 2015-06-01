/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/jplace_processor.hpp"
#include "placement/placement_map.hpp"

/*
void BoostPythonExport_JplaceProcessor()
{
    // -------------------------------------------------------------------
    //     Class JplaceProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "GetVersion",
            ( std::string ( * )(  ))( &::genesis::JplaceProcessor::GetVersion ),
            "Returns the version number that this class is written for."
        )
        .staticmethod("GetVersion")
        .def(
            "CheckVersion",
            ( bool ( * )( const std::string ))( &::genesis::JplaceProcessor::CheckVersion ),
            ( boost::python::arg("version") ),
            "Checks whether the version of the jplace format works with this parser."
        )
        .staticmethod("CheckVersion")
        .def(
            "from_file",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("from_file")
        .def(
            "from_string",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::from_string ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("from_string")
        .def(
            "FromDocument",
            ( bool ( * )( const ::genesis::JsonDocument &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") ),
            "Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromDocument")
        .def(
            "to_file",
            ( bool ( * )( const std::string, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_file ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("to_file")
        .def(
            "to_string",
            ( void ( * )( std::string &, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_string ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") )
        )
        .staticmethod("to_string")
        .def(
            "to_string",
            ( std::string ( * )( const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_string ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("to_string")
        .def(
            "to_document",
            ( void ( * )( ::genesis::JsonDocument &, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_document ),
            ( boost::python::arg("doc"), boost::python::arg("placements") )
        )
        .staticmethod("to_document")
    ;
}
*/

void BoostPythonExport_JplaceProcessor()
{

    boost::python::class_< ::genesis::JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "get_version",
            ( std::string ( * )(  ))( &::genesis::JplaceProcessor::get_version ),
            "Returns the version number that this class is written for."
        )
        .staticmethod("get_version")
        .def(
            "check_version",
            ( bool ( * )( const std::string ))( &::genesis::JplaceProcessor::check_version ),
            ( boost::python::arg("version") ),
            "Checks whether the version of the jplace format works with this parser."
        )
        .staticmethod("check_version")
        .def(
            "from_file",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::from_file ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("from_file")
        .def(
            "from_string",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::from_string ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("from_string")

        .def(
            "to_file",
            ( bool ( * )( const std::string, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_file ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("to_file")
        .def(
            "to_string",
            ( std::string ( * )( const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::to_string ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("to_string")
    ;

}
