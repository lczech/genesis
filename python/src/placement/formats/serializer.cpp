/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::SampleSerializer, scope )
{

    // -------------------------------------------------------------------
    //     Class SampleSerializer
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SampleSerializer, std::shared_ptr<::genesis::placement::SampleSerializer> > ( scope, "SampleSerializer" )

        // Public Member Functions

        .def_static(
            "load",
            ( Sample ( * )( std::string const & ))( &::genesis::placement::SampleSerializer::load ),
            pybind11::arg("file_name")
        )
        .def_static(
            "save",
            ( void ( * )( Sample const &, std::string const & ))( &::genesis::placement::SampleSerializer::save ),
            pybind11::arg("map"),
            pybind11::arg("file_name")
        )
    ;
}
