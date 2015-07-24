/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "placement/placement_map.hpp"
#include "placement/serializer.hpp"

PYTHON_EXPORT_CLASS (PlacementMapSerializer, "placement")
{
    // -------------------------------------------------------------------
    //     Class PlacementMapSerializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::PlacementMapSerializer > ( "PlacementMapSerializer" )

        // Public Member Functions

        .def(
            "load",
            ( bool ( * )( const std::string &, ::genesis::PlacementMap & ))( &::genesis::PlacementMapSerializer::load ),
            ( boost::python::arg("file_name"), boost::python::arg("map") ),
            get_docstring("static bool ::genesis::PlacementMapSerializer::load (const std::string & file_name, PlacementMap & map)")
        )
        .staticmethod("load")
        .def(
            "save",
            ( bool ( * )( const ::genesis::PlacementMap &, const std::string & ))( &::genesis::PlacementMapSerializer::save ),
            ( boost::python::arg("map"), boost::python::arg("file_name") ),
            get_docstring("static bool ::genesis::PlacementMapSerializer::save (const PlacementMap & map, const std::string & file_name)")
        )
        .staticmethod("save")
    ;
}
