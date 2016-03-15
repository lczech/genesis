/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/placement/sample.hpp"
#include "lib/placement/io/serializer.hpp"

using namespace genesis::placement;

PYTHON_EXPORT_CLASS (SampleSerializer, "placement")
{
    // -------------------------------------------------------------------
    //     Class SampleSerializer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::placement::SampleSerializer > ( "SampleSerializer" )

        // Public Member Functions

        .def(
            "load",
            ( bool ( * )( const std::string &, ::genesis::placement::Sample & ))( &::genesis::placement::SampleSerializer::load ),
            ( boost::python::arg("file_name"), boost::python::arg("map") ),
            get_docstring("static bool ::genesis::placement::SampleSerializer::load (const std::string & file_name, Sample & map)")
        )
        .staticmethod("load")
        .def(
            "save",
            ( bool ( * )( const ::genesis::placement::Sample &, const std::string & ))( &::genesis::placement::SampleSerializer::save ),
            ( boost::python::arg("map"), boost::python::arg("file_name") ),
            get_docstring("static bool ::genesis::placement::SampleSerializer::save (const Sample & map, const std::string & file_name)")
        )
        .staticmethod("save")
    ;
}
