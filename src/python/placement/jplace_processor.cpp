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
            "FromFile",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromString")
        .def(
            "FromDocument",
            ( bool ( * )( const ::genesis::JsonDocument &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") ),
            "Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromDocument")
        .def(
            "ToFile",
            ( bool ( * )( const std::string, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( void ( * )( std::string &, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToString",
            ( std::string ( * )( const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToString ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("ToString")
        .def(
            "ToDocument",
            ( void ( * )( ::genesis::JsonDocument &, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToDocument ),
            ( boost::python::arg("doc"), boost::python::arg("placements") )
        )
        .staticmethod("ToDocument")
    ;
}
*/

void BoostPythonExport_JplaceProcessor()
{

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
            "FromFile",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::FromString ),
            ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            "Parses a string as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromString")

        .def(
            "ToFile",
            ( bool ( * )( const std::string, const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("placements") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( std::string ( * )( const ::genesis::PlacementMap & ))( &::genesis::JplaceProcessor::ToString ),
            ( boost::python::arg("placements") )
        )
        .staticmethod("ToString")
    ;

}
