/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/jplace_processor.hpp"
#include "placement/placement_map.hpp"

void BoostPythonExport_JplaceProcessor()
{
    using namespace boost::python;
    using namespace genesis;

    boost::python::class_< JplaceProcessor > ( "JplaceProcessor" )

        // Public Member Functions

        .def(
            "GetVersion",
            &JplaceProcessor::GetVersion,
            "Returns the version number that this class is written for."
        )
        .staticmethod("GetVersion")
        .def(
            "CheckVersion",
            &JplaceProcessor::CheckVersion,
            ( boost::python::arg("version") ),
            "Checks whether the version of the jplace format works with this parser."
        )
        .staticmethod("CheckVersion")
        .def(
            "FromFile",
            //~ ( bool ( JplaceProcessor::* )( const std::string &, PlacementMap & ))( &JplaceProcessor::FromFile ),
            &JplaceProcessor::FromFile,
            ( boost::python::arg("fn"), boost::python::arg("placements") ),
            "Reads a file and parses it as a Jplace document into a PlacementMap object."
        )
        .staticmethod("FromFile")
        //~ .def(
            //~ "FromString",
            //~ &JplaceProcessor::FromString,
            //~ ( boost::python::arg("jplace"), boost::python::arg("placements") ),
            //~ "Parses a string as a Jplace document into a PlacementMap object."
        //~ )
        //~ .staticmethod("FromString")
        //~ .def(
            //~ "FromDocument",
            //~ &JplaceProcessor::FromDocument,
            //~ ( boost::python::arg("doc"), boost::python::arg("placements") ),
            //~ "Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object."
        //~ )
        //~ .staticmethod("FromDocument")
        //~ .def(
            //~ "ToFile",
            //~ &JplaceProcessor::ToFile,
            //~ ( boost::python::arg("fn"), boost::python::arg("placements") )
        //~ )
        //~ .staticmethod("ToFile")
        //~ .def(
            //~ "ToString",
            //~ &JplaceProcessor::ToString,
            //~ ( boost::python::arg("jplace"), boost::python::arg("placements") )
        //~ )
        //~ .staticmethod("ToString")
        //~ .def(
            //~ "ToString",
            //~ &JplaceProcessor::ToString,
            //~ ( boost::python::arg("placements") )
        //~ )
        //~ .staticmethod("ToString")
        //~ .def(
            //~ "ToDocument",
            //~ &JplaceProcessor::ToDocument,
            //~ ( boost::python::arg("doc"), boost::python::arg("placements") )
        //~ )
        //~ .staticmethod("ToDocument")
    ;

}
