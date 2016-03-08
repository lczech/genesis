/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/placement_map.hpp"
#include "lib/placement/io/serializer.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (PlacementMapSerializer, "placement")
{
    // -------------------------------------------------------------------
    //     Class PlacementMapSerializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::PlacementMapSerializer > ( "PlacementMapSerializer" )

        // Public Member Functions

        .def(
            "load",
            ( bool ( * )( const std::string &, ::genesis::placement::PlacementMap & ))( &::genesis::placement::PlacementMapSerializer::load ),
            ( boost::python::arg("file_name"), boost::python::arg("map") ),
            get_docstring("static bool ::genesis::placement::PlacementMapSerializer::load (const std::string & file_name, PlacementMap & map)")
        )
        .staticmethod("load")
        .def(
            "save",
            ( bool ( * )( const ::genesis::placement::PlacementMap &, const std::string & ))( &::genesis::placement::PlacementMapSerializer::save ),
            ( boost::python::arg("map"), boost::python::arg("file_name") ),
            get_docstring("static bool ::genesis::placement::PlacementMapSerializer::save (const PlacementMap & map, const std::string & file_name)")
        )
        .staticmethod("save")
    ;
}
